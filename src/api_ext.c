#define _POSIX_C_SOURCE 200809L
#include "api_ext.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int ascii_ieq(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (tolower(ca) != tolower(cb)) {
            return 0;
        }
    }
    return 1;
}

static void copy_n_term(char *dst, size_t cap, const char *src, size_t len) {
    if (!dst || cap == 0) {
        return;
    }
    if (!src || len == 0) {
        dst[0] = '\0';
        return;
    }
    size_t n = len < (cap - 1) ? len : (cap - 1);
    memcpy(dst, src, n);
    dst[n] = '\0';
}

/* 1. Case-insensitive HTTP header lookup */
int cjang_ext_header_get(const char *raw_req, const char *name, char *out, size_t out_cap) {
    if (!raw_req || !name || !name[0] || !out || out_cap == 0) {
        return -1;
    }
    size_t nlen = strlen(name);
    const char *line = strstr(raw_req, "\r\n");
    if (!line) {
        line = strchr(raw_req, '\n');
        if (!line) {
            return -1;
        }
        line += 1;
    } else {
        line += 2;
    }

    while (*line != '\0') {
        if (line[0] == '\r' || line[0] == '\n') {
            break;
        }
        const char *eol = strstr(line, "\r\n");
        const char *next = NULL;
        size_t llen = 0;
        if (eol) {
            llen = (size_t)(eol - line);
            next = eol + 2;
        } else {
            const char *nl = strchr(line, '\n');
            if (nl) {
                llen = (size_t)(nl - line);
                next = nl + 1;
            } else {
                llen = strlen(line);
                next = line + llen;
            }
        }

        const char *colon = memchr(line, ':', llen);
        if (colon != NULL) {
            size_t hlen = (size_t)(colon - line);
            if (hlen == nlen && ascii_ieq(line, name, nlen)) {
                const char *v = colon + 1;
                const char *vend = line + llen;
                while (v < vend && (*v == ' ' || *v == '\t')) {
                    v++;
                }
                while (vend > v && (vend[-1] == ' ' || vend[-1] == '\t')) {
                    vend--;
                }
                size_t vlen = (size_t)(vend - v);
                copy_n_term(out, out_cap, v, vlen);
                return (int)(vlen < (out_cap - 1) ? vlen : (out_cap - 1));
            }
        }
        line = next;
    }
    return -1;
}

/* 2. Cookie lookup */
int cjang_ext_cookie_get(const char *cookie_hdr, const char *name, char *out, size_t out_cap) {
    if (!cookie_hdr || !name || !name[0] || !out || out_cap == 0) {
        return -1;
    }
    size_t nlen = strlen(name);
    const char *p = cookie_hdr;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == ';' || *p == '\r' || *p == '\n') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        const char *end = p;
        while (*end != '\0' && *end != ';' && *end != '\r' && *end != '\n') {
            end++;
        }
        const char *eq = memchr(p, '=', (size_t)(end - p));
        if (eq != NULL) {
            size_t klen = (size_t)(eq - p);
            if (klen == nlen && strncmp(p, name, nlen) == 0) {
                const char *val = eq + 1;
                const char *vend = end;
                while (vend > val && (vend[-1] == ' ' || vend[-1] == '\t')) {
                    vend--;
                }
                size_t vlen = (size_t)(vend - val);
                copy_n_term(out, out_cap, val, vlen);
                return (int)(vlen < (out_cap - 1) ? vlen : (out_cap - 1));
            }
        }
        p = end;
    }
    return -1;
}

/* 3. Allowed method check */
int cjang_ext_method_allowed(const char *method, const char *allow_list) {
    if (!method || !method[0] || !allow_list || !allow_list[0]) {
        return 0;
    }
    size_t mlen = strlen(method);
    const char *p = allow_list;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == ',') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        const char *start = p;
        while (*p != '\0' && *p != ',' && *p != ' ' && *p != '\t') {
            p++;
        }
        size_t tok_len = (size_t)(p - start);
        if (tok_len == mlen && strncmp(start, method, mlen) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 4. Path cleaning & traversal guard */
int cjang_ext_path_clean(const char *raw_path, char *out, size_t out_cap) {
    if (!raw_path || raw_path[0] != '/' || !out || out_cap < 2) {
        return -1;
    }

    size_t pos = 0;
    out[pos++] = '/';

    const char *p = raw_path;
    while (*p != '\0' && *p != '?') {
        while (*p == '/') {
            p++;
        }
        if (*p == '\0' || *p == '?') {
            break;
        }
        const char *seg_start = p;
        while (*p != '\0' && *p != '/' && *p != '?') {
            p++;
        }
        size_t slen = (size_t)(p - seg_start);
        if (slen == 1 && seg_start[0] == '.') {
            continue;
        }
        if (slen == 2 && seg_start[0] == '.' && seg_start[1] == '.') {
            return -1;
        }
        if (pos > 1) {
            if (pos + 1 >= out_cap) {
                return -1;
            }
            out[pos++] = '/';
        }
        if (pos + slen >= out_cap) {
            return -1;
        }
        memcpy(out + pos, seg_start, slen);
        pos += slen;
    }

    out[pos] = '\0';
    return 0;
}

/* 5. JSON string escaping */
size_t cjang_ext_json_escape(const char *src, char *dst, size_t dst_cap) {
    if (!dst || dst_cap == 0) {
        return 0;
    }
    if (!src) {
        dst[0] = '\0';
        return 0;
    }

    size_t w = 0;
    for (size_t i = 0; src[i] != '\0'; i++) {
        unsigned char c = (unsigned char)src[i];
        const char *esc = NULL;
        if (c == '"') {
            esc = "\\\"";
        } else if (c == '\\') {
            esc = "\\\\";
        } else if (c == '\n') {
            esc = "\\n";
        } else if (c == '\r') {
            esc = "\\r";
        } else if (c == '\t') {
            esc = "\\t";
        }

        if (esc != NULL) {
            size_t elen = strlen(esc);
            if (w + elen >= dst_cap) {
                break;
            }
            memcpy(dst + w, esc, elen);
            w += elen;
        } else {
            if (w + 1 >= dst_cap) {
                break;
            }
            dst[w++] = (char)c;
        }
    }

    dst[w] = '\0';
    return w;
}

/* 6. Flat JSON int64 extractor */
int cjang_ext_json_get_int64(const char *json, size_t json_len, const char *key, int64_t *out_val) {
    if (!json || json_len == 0 || !key || !key[0] || !out_val) {
        return -1;
    }
    size_t klen = strlen(key);
    for (size_t i = 0; i + klen + 2 < json_len; i++) {
        if (json[i] == '"' && strncmp(json + i + 1, key, klen) == 0 && json[i + 1 + klen] == '"') {
            size_t p = i + 2 + klen;
            while (p < json_len && (json[p] == ' ' || json[p] == '\t')) {
                p++;
            }
            if (p >= json_len || json[p] != ':') {
                continue;
            }
            p++;
            while (p < json_len && (json[p] == ' ' || json[p] == '\t')) {
                p++;
            }
            if (p >= json_len) {
                return -1;
            }
            char num_buf[32];
            size_t nlen = 0;
            while (p + nlen < json_len && nlen + 1 < sizeof(num_buf)) {
                char c = json[p + nlen];
                if ((c >= '0' && c <= '9') || (nlen == 0 && (c == '-' || c == '+'))) {
                    num_buf[nlen++] = c;
                } else {
                    break;
                }
            }
            if (nlen == 0) {
                return -1;
            }
            num_buf[nlen] = '\0';
            errno = 0;
            char *endp = NULL;
            long long v = strtoll(num_buf, &endp, 10);
            if (errno != 0 || endp == num_buf || *endp != '\0') {
                return -1;
            }
            *out_val = (int64_t)v;
            return 0;
        }
    }
    return -1;
}

/* 7. HTTP/1.1 response builder with X-Cjang-Us */
size_t cjang_ext_build_http_status(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *reason,
    const char *content_type,
    const char *body,
    uint64_t elapsed_us
) {
    if (!dst || dst_cap == 0) {
        return 0;
    }
    const char *rsn = (reason && reason[0]) ? reason : "OK";
    const char *ct = (content_type && content_type[0]) ? content_type : "text/plain; charset=utf-8";
    const char *bd = body ? body : "";
    size_t blen = strlen(bd);

    int n = snprintf(
        dst,
        dst_cap,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "X-Cjang-Us: %llu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        rsn,
        ct,
        blen,
        (unsigned long long)elapsed_us,
        bd
    );
    if (n < 0) {
        dst[0] = '\0';
        return 0;
    }
    size_t written = (size_t)n;
    return written < dst_cap ? written : (dst_cap - 1);
}

/* 8. Monotonic fixed-window rate limiter */
void cjang_ext_rate_init(cjang_ext_rate_t *rl, uint32_t max_per_window, uint64_t window_sec) {
    if (!rl) {
        return;
    }
    memset(rl, 0, sizeof(*rl));
    rl->max_per_window = max_per_window;
    rl->window_sec = window_sec > 0 ? window_sec : 1;
}

int cjang_ext_rate_allow(cjang_ext_rate_t *rl) {
    if (!rl || rl->max_per_window == 0) {
        return 0;
    }
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 1;
    }
    uint64_t now_sec = (uint64_t)ts.tv_sec;
    if (rl->window_start_sec == 0 || now_sec >= rl->window_start_sec + rl->window_sec) {
        rl->window_start_sec = now_sec;
        rl->count_in_window = 1;
        return 1;
    }
    if (rl->count_in_window < rl->max_per_window) {
        rl->count_in_window++;
        return 1;
    }
    return 0;
}

/* 9. Key-value line parser */
int cjang_ext_parse_kv_line(
    const char *line,
    char *key_out,
    size_t key_cap,
    char *val_out,
    size_t val_cap
) {
    if (!line || !key_out || key_cap == 0 || !val_out || val_cap == 0) {
        return -1;
    }
    const char *p = line;
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    const char *sep = p;
    while (*sep != '\0' && *sep != '=' && *sep != ':' && *sep != '\n' && *sep != '\r') {
        sep++;
    }
    if (*sep != '=' && *sep != ':') {
        return -1;
    }
    const char *kend = sep;
    while (kend > p && (kend[-1] == ' ' || kend[-1] == '\t')) {
        kend--;
    }
    if (kend <= p) {
        return -1;
    }
    const char *v = sep + 1;
    while (*v == ' ' || *v == '\t') {
        v++;
    }
    const char *vend = v;
    while (*vend != '\0' && *vend != '\r' && *vend != '\n') {
        vend++;
    }
    while (vend > v && (vend[-1] == ' ' || vend[-1] == '\t')) {
        vend--;
    }

    copy_n_term(key_out, key_cap, p, (size_t)(kend - p));
    copy_n_term(val_out, val_cap, v, (size_t)(vend - v));
    return 0;
}

/* 10. Compact metrics key=value snapshot */
size_t cjang_ext_metrics_snapshot_kv(const cjang_metrics_t *m, char *out, size_t out_cap) {
    if (!out || out_cap == 0) {
        return 0;
    }
    uint64_t tot = m ? m->total_requests : 0;
    uint64_t ok = m ? m->ok_200 : 0;
    uint64_t c4 = m ? m->client_4xx : 0;
    uint64_t s5 = m ? m->server_5xx : 0;
    uint64_t us = m ? m->last_exec_us : 0;

    int n = snprintf(
        out,
        out_cap,
        "total_requests=%llu\nok_200=%llu\nclient_4xx=%llu\nserver_5xx=%llu\nlast_exec_us=%llu\n",
        (unsigned long long)tot,
        (unsigned long long)ok,
        (unsigned long long)c4,
        (unsigned long long)s5,
        (unsigned long long)us
    );
    if (n < 0) {
        out[0] = '\0';
        return 0;
    }
    size_t w = (size_t)n;
    return w < out_cap ? w : (out_cap - 1);
}
