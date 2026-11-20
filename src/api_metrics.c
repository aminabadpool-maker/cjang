#define _POSIX_C_SOURCE 199309L
#include "api_metrics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t diff_us(struct timespec start, struct timespec end) {
    if (end.tv_sec < start.tv_sec) {
        return 0;
    }
    uint64_t sec_diff = (uint64_t)(end.tv_sec - start.tv_sec);
    int64_t nsec_diff = (int64_t)end.tv_nsec - (int64_t)start.tv_nsec;
    int64_t total_ns = (int64_t)(sec_diff * 1000000000ULL) + nsec_diff;
    if (total_ns <= 0) {
        return 0;
    }
    return (uint64_t)total_ns / 1000ULL;
}

static int parse_http_status_code(const char *resp) {
    if (!resp) {
        return 0;
    }
    const char *sp = strchr(resp, ' ');
    if (!sp) {
        return 0;
    }
    while (*sp == ' ') {
        sp++;
    }
    if (*sp < '0' || *sp > '9') {
        return 0;
    }
    return atoi(sp);
}

void cjang_metrics_reset(cjang_metrics_t *m) {
    if (!m) {
        return;
    }
    memset(m, 0, sizeof(*m));
}

size_t cjang_dispatch_metered(
    cjang_metrics_t *m,
    const cjang_route_t *routes,
    size_t route_count,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
) {
    if (!http_out || http_cap == 0) {
        return 0;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    size_t n = cjang_dispatch_request(
        routes,
        route_count,
        raw_req,
        body,
        body_len,
        http_out,
        http_cap
    );

    clock_gettime(CLOCK_MONOTONIC, &t1);
    uint64_t us = diff_us(t0, t1);

    if (n > 0 && http_out[n - 1] == '\0') {
        n--;
    }

    char extra_hdr[128];
    int hlen = snprintf(
        extra_hdr,
        sizeof(extra_hdr),
        "X-Cjang-Us: %llu\r\nX-Cjang-Bytes: %zu\r\n",
        (unsigned long long)us,
        n
    );

    if (hlen > 0 && (size_t)hlen < sizeof(extra_hdr) && n + (size_t)hlen < http_cap) {
        char *sep = strstr(http_out, "\r\n\r\n");
        if (sep != NULL) {
            size_t prefix_len = (size_t)((sep + 2) - http_out);
            size_t suffix_len = n - prefix_len;
            memmove(http_out + prefix_len + (size_t)hlen, http_out + prefix_len, suffix_len);
            memcpy(http_out + prefix_len, extra_hdr, (size_t)hlen);
            n += (size_t)hlen;
            http_out[n] = '\0';
        }
    }

    if (m != NULL) {
        m->total_requests++;
        m->total_resp_bytes += (uint64_t)n;
        m->last_exec_us = us;
        if (us > m->max_exec_us) {
            m->max_exec_us = us;
        }
        int code = parse_http_status_code(http_out);
        if (code == 200) {
            m->ok_200++;
        } else if (code >= 400 && code < 500) {
            m->client_4xx++;
        } else if (code >= 500) {
            m->server_5xx++;
        }
    }

    return n;
}
