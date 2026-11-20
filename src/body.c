#include "cjang.h"
#include <stdio.h>
#include <string.h>

static int copy_bounded(const char *src, size_t len, char *out, size_t out_sz) {
    if (!out || out_sz == 0) return 0;
    if (!src) {
        out[0] = '\0';
        return 0;
    }
    if (len >= out_sz) {
        len = out_sz - 1;
    }
    memcpy(out, src, len);
    out[len] = '\0';
    return 1;
}

int cjang_form_get(const cjang_request_t *req, const char *key, char *out, size_t out_sz) {
    if (!req || !key || *key == '\0' || !out || out_sz == 0) return 0;
    out[0] = '\0';
    if (req->body_len == 0) return 0;

    size_t k_len = strlen(key);
    const char *p = req->body;
    const char *end = req->body + req->body_len;

    while (p < end) {
        const char *amp = memchr(p, '&', (size_t)(end - p));
        const char *pair_end = amp ? amp : end;

        const char *eq = memchr(p, '=', (size_t)(pair_end - p));
        if (eq) {
            size_t cur_k_len = (size_t)(eq - p);
            if (cur_k_len == k_len && memcmp(p, key, k_len) == 0) {
                const char *val_start = eq + 1;
                size_t val_len = (size_t)(pair_end - val_start);
                return copy_bounded(val_start, val_len, out, out_sz);
            }
        }

        if (!amp) break;
        p = amp + 1;
    }

    return 0;
}

int cjang_json_get_str(const cjang_request_t *req, const char *key, char *out, size_t out_sz) {
    if (!req || !key || *key == '\0' || !out || out_sz == 0) return 0;
    out[0] = '\0';
    if (req->body_len == 0) return 0;

    char needle[CJANG_QUERY_KEY_LEN + 4];
    int n_len = snprintf(needle, sizeof(needle), "\"%s\"", key);
    if (n_len <= 0 || (size_t)n_len >= sizeof(needle)) return 0;

    const char *scan = req->body;
    const char *end = req->body + req->body_len;

    while (scan < end) {
        const char *found = strstr(scan, needle);
        if (!found || found >= end) return 0;

        const char *after_key = found + (size_t)n_len;
        while (after_key < end && (*after_key == ' ' || *after_key == '\t' ||
                                  *after_key == '\r' || *after_key == '\n')) {
            after_key++;
        }

        if (after_key < end && *after_key == ':') {
            const char *val_pos = after_key + 1;
            while (val_pos < end && (*val_pos == ' ' || *val_pos == '\t' ||
                                     *val_pos == '\r' || *val_pos == '\n')) {
                val_pos++;
            }

            if (val_pos < end && *val_pos == '"') {
                const char *str_start = val_pos + 1;
                const char *str_end = str_start;
                while (str_end < end && *str_end != '"') {
                    if (*str_end == '\\' && (str_end + 1) < end) {
                        str_end += 2;
                        continue;
                    }
                    str_end++;
                }
                if (str_end < end && *str_end == '"') {
                    return copy_bounded(str_start, (size_t)(str_end - str_start), out, out_sz);
                }
            }
            return 0;
        }

        scan = after_key;
    }

    return 0;
}
