#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_json_escape(const char *src, char *dst, size_t dst_cap) {
    if (!dst || dst_cap == 0) return 0;
    dst[0] = '\0';
    if (!src) return 0;

    size_t out_len = 0;
    for (size_t i = 0; src[i] != '\0'; i++) {
        unsigned char c = (unsigned char)src[i];
        const char *rep = NULL;
        char single[2] = {0, 0};

        if (c == '"') rep = "\\\"";
        else if (c == '\\') rep = "\\\\";
        else if (c == '\n') rep = "\\n";
        else if (c == '\r') rep = "\\r";
        else if (c == '\t') rep = "\\t";
        else if (c >= 0x20) {
            single[0] = (char)c;
            rep = single;
        } else {
            continue;
        }

        size_t rlen = strlen(rep);
        if (out_len + rlen >= dst_cap) break;
        memcpy(dst + out_len, rep, rlen);
        out_len += rlen;
    }

    dst[out_len] = '\0';
    return out_len;
}

size_t cj_http_build_json_kv(
    int status_code,
    const char *status_text,
    const char *key,
    const char *val,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    const char *k = (key && key[0] != '\0') ? key : "status";
    const char *v = val ? val : "";

    char esc_k[CJ_JSON_ESC_MAX];
    char esc_v[CJ_JSON_ESC_MAX];
    cj_json_escape(k, esc_k, sizeof(esc_k));
    cj_json_escape(v, esc_v, sizeof(esc_v));

    char json_body[CJ_JSON_ESC_MAX * 2 + 32];
    int n = snprintf(json_body, sizeof(json_body), "{\"%s\":\"%s\"}", esc_k, esc_v);
    if (n <= 0 || (size_t)n >= sizeof(json_body)) return 0;

    return cj_http_build_text_resp(
        status_code,
        status_text,
        "application/json; charset=utf-8",
        json_body,
        out_buf,
        out_cap
    );
}
