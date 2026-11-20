#define _POSIX_C_SOURCE 200809L
#include "api_plus.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ieq_n(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }
    }
    return 1;
}

int cjang_plus_bearer_get(const char *raw_req, char *out, size_t out_cap) {
    if (!raw_req || !out || out_cap < 2) {
        return -1;
    }
    char auth[512];
    if (cjang_ext_header_get(raw_req, "authorization", auth, sizeof(auth)) <= 0) {
        return -1;
    }
    const char *p = auth;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (strlen(p) < 7 || !ieq_n(p, "bearer", 6) || (p[6] != ' ' && p[6] != '\t')) {
        return -1;
    }
    p += 7;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '\0') {
        return -1;
    }
    size_t tlen = strlen(p);
    size_t n = tlen < (out_cap - 1) ? tlen : (out_cap - 1);
    memcpy(out, p, n);
    out[n] = '\0';
    return (int)n;
}

int cjang_plus_param_get_i64(const char *kv_str, size_t kv_len, const char *key, int64_t *out_val) {
    if (!kv_str || kv_len == 0 || !key || !key[0] || !out_val) {
        return -1;
    }
    size_t klen = strlen(key);
    size_t i = 0;
    while (i < kv_len) {
        size_t amp = i;
        while (amp < kv_len && kv_str[amp] != '&') {
            amp++;
        }
        size_t eq = i;
        while (eq < amp && kv_str[eq] != '=') {
            eq++;
        }
        if (eq < amp && (eq - i) == klen && strncmp(kv_str + i, key, klen) == 0) {
            size_t vlen = amp - (eq + 1);
            if (vlen == 0 || vlen >= 32) {
                return -1;
            }
            char buf[32];
            memcpy(buf, kv_str + eq + 1, vlen);
            buf[vlen] = '\0';
            errno = 0;
            char *endp = NULL;
            long long val = strtoll(buf, &endp, 10);
            if (errno != 0 || endp == buf || *endp != '\0') {
                return -1;
            }
            *out_val = (int64_t)val;
            return 0;
        }
        i = (amp < kv_len) ? (amp + 1) : kv_len;
    }
    return -1;
}

int cjang_plus_json_get_str(const char *json, size_t json_len, const char *key, char *out, size_t out_cap) {
    if (!json || json_len == 0 || !key || !key[0] || !out || out_cap == 0) {
        return -1;
    }
    size_t klen = strlen(key);
    for (size_t i = 0; i + klen + 4 < json_len; i++) {
        if (json[i] == '"' && strncmp(json + i + 1, key, klen) == 0 && json[i + 1 + klen] == '"') {
            size_t p = i + 2 + klen;
            while (p < json_len && (json[p] == ' ' || json[p] == '\t' || json[p] == '\n' || json[p] == '\r')) {
                p++;
            }
            if (p >= json_len || json[p] != ':') {
                continue;
            }
            p++;
            while (p < json_len && (json[p] == ' ' || json[p] == '\t' || json[p] == '\n' || json[p] == '\r')) {
                p++;
            }
            if (p >= json_len || json[p] != '"') {
                return -1;
            }
            p++;
            size_t w = 0;
            while (p < json_len && json[p] != '"') {
                char c = json[p++];
                if (c == '\\' && p < json_len) {
                    char esc = json[p++];
                    if (esc == 'n') c = '\n';
                    else if (esc == 'r') c = '\r';
                    else if (esc == 't') c = '\t';
                    else c = esc;
                }
                if (w + 1 < out_cap) {
                    out[w++] = c;
                }
            }
            if (p >= json_len || json[p] != '"') {
                return -1;
            }
            out[w] = '\0';
            return (int)w;
        }
    }
    return -1;
}

size_t cjang_plus_trim_inplace(char *s) {
    if (!s) {
        return 0;
    }
    size_t len = strlen(s);
    size_t start = 0;
    while (start < len && isspace((unsigned char)s[start])) {
        start++;
    }
    size_t end = len;
    while (end > start && isspace((unsigned char)s[end - 1])) {
        end--;
    }
    size_t nlen = end - start;
    if (start > 0 && nlen > 0) {
        memmove(s, s + start, nlen);
    }
    s[nlen] = '\0';
    return nlen;
}

int cjang_plus_str_append(char *dst, size_t dst_cap, const char *add) {
    if (!dst || dst_cap == 0 || !add) {
        return -1;
    }
    size_t cur = strnlen(dst, dst_cap);
    if (cur >= dst_cap) {
        return -1;
    }
    size_t alen = strlen(add);
    size_t avail = dst_cap - 1 - cur;
    size_t copy_n = alen < avail ? alen : avail;
    if (copy_n > 0) {
        memcpy(dst + cur, add, copy_n);
    }
    dst[cur + copy_n] = '\0';
    return (alen <= avail) ? 0 : -1;
}

const char *cjang_plus_status_reason(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        default:  return "OK";
    }
}

size_t cjang_plus_build_json_resp(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *json_body
) {
    if (!dst || dst_cap == 0) {
        return 0;
    }
    const char *body = json_body ? json_body : "{}";
    size_t blen = strlen(body);
    const char *reason = cjang_plus_status_reason(status_code);
    int n = snprintf(
        dst,
        dst_cap,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        reason,
        blen,
        body
    );
    if (n < 0) {
        dst[0] = '\0';
        return 0;
    }
    size_t w = (size_t)n;
    return w < dst_cap ? w : (dst_cap - 1);
}

int cjang_plus_route_match_idx(
    const cjang_route_t *routes,
    size_t route_count,
    const char *method,
    const char *path
) {
    if (!routes || route_count == 0 || !method || !path) {
        return -1;
    }
    for (size_t i = 0; i < route_count; i++) {
        if (routes[i].method && routes[i].pattern &&
            strcmp(routes[i].method, method) == 0 &&
            strcmp(routes[i].pattern, path) == 0) {
            return (int)i;
        }
    }
    return -1;
}

int cjang_plus_path_has_allowed_method(
    const cjang_route_t *routes,
    size_t route_count,
    const char *path,
    const char *method
) {
    return cjang_plus_route_match_idx(routes, route_count, method, path) >= 0 ? 1 : 0;
}

uint64_t cjang_plus_elapsed_us_since(const struct timespec *start_ts) {
    if (!start_ts) {
        return 0;
    }
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return 0;
    }
    if (now.tv_sec < start_ts->tv_sec ||
        (now.tv_sec == start_ts->tv_sec && now.tv_nsec < start_ts->tv_nsec)) {
        return 0;
    }
    uint64_t sec_diff = (uint64_t)(now.tv_sec - start_ts->tv_sec);
    int64_t nsec_diff = now.tv_nsec - start_ts->tv_nsec;
    uint64_t total_us = sec_diff * 1000000ULL;
    if (nsec_diff > 0) {
        total_us += (uint64_t)(nsec_diff / 1000L);
    }
    return total_us;
}
