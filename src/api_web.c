#include "api_web.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ascii_str_eq_ci(const char *a, const char *b) {
    if (!a || !b) {
        return 0;
    }
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static const char *find_var_val(const cjang_web_kv_t *vars, size_t var_count, const char *key, size_t klen) {
    if (!vars || var_count == 0 || !key || klen == 0) {
        return NULL;
    }
    for (size_t i = 0; i < var_count; i++) {
        if (vars[i].key && strlen(vars[i].key) == klen && strncmp(vars[i].key, key, klen) == 0) {
            return vars[i].val ? vars[i].val : "";
        }
    }
    return NULL;
}

int cjang_web_check_method_or_405(
    const char *method,
    const char *allow_list,
    char *resp_out,
    size_t resp_cap
) {
    if (cjang_ext_method_allowed(method, allow_list)) {
        return 1;
    }
    if (!resp_out || resp_cap == 0) {
        return 0;
    }
    const char *allow = (allow_list && allow_list[0]) ? allow_list : "GET, POST";
    const char *body = "method_not_allowed\n";
    int n = snprintf(
        resp_out,
        resp_cap,
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Allow: %s\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        allow,
        strlen(body),
        body
    );
    if (n < 0) {
        resp_out[0] = '\0';
    }
    return 0;
}

int cjang_web_param_get_decoded(
    const char *kv_str,
    size_t kv_len,
    const char *key,
    char *out,
    size_t out_cap
) {
    if (!kv_str || kv_len == 0 || !key || !key[0] || !out || out_cap == 0) {
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
            const char *raw_val = kv_str + eq + 1;
            size_t raw_len = amp - (eq + 1);
            return (int)cjang_url_decode(raw_val, raw_len, out, out_cap);
        }
        i = (amp < kv_len) ? (amp + 1) : kv_len;
    }

    return -1;
}

int cjang_web_parse_content_length(
    const char *raw_req,
    size_t max_body_len,
    size_t *out_len
) {
    if (!raw_req || !out_len) {
        return -1;
    }
    char hdr_val[32];
    int rc = cjang_ext_header_get(raw_req, "content-length", hdr_val, sizeof(hdr_val));
    if (rc <= 0) {
        *out_len = 0;
        return 0;
    }
    for (int i = 0; i < rc; i++) {
        if (!isdigit((unsigned char)hdr_val[i])) {
            return -1;
        }
    }
    errno = 0;
    char *endp = NULL;
    unsigned long long v = strtoull(hdr_val, &endp, 10);
    if (errno != 0 || endp == hdr_val || *endp != '\0') {
        return -1;
    }
    if ((size_t)v > max_body_len) {
        return -1;
    }
    *out_len = (size_t)v;
    return 0;
}

size_t cjang_web_render_template_ex(
    const char *tmpl,
    const cjang_web_kv_t *vars,
    size_t var_count,
    int json_escape,
    char *out,
    size_t out_cap
) {
    if (!out || out_cap == 0) {
        return 0;
    }
    if (!tmpl) {
        out[0] = '\0';
        return 0;
    }

    size_t w = 0;
    size_t i = 0;

    while (tmpl[i] != '\0') {
        if (tmpl[i] == '{' && tmpl[i + 1] == '{') {
            const char *close = strstr(tmpl + i + 2, "}}");
            if (close != NULL) {
                size_t klen = (size_t)(close - (tmpl + i + 2));
                const char *val = find_var_val(vars, var_count, tmpl + i + 2, klen);
                if (val != NULL) {
                    if (json_escape) {
                        char esc_buf[512];
                        size_t elen = cjang_ext_json_escape(val, esc_buf, sizeof(esc_buf));
                        size_t copy_n = (w + elen < out_cap) ? elen : (out_cap - 1 - w);
                        memcpy(out + w, esc_buf, copy_n);
                        w += copy_n;
                    } else {
                        size_t vlen = strlen(val);
                        size_t copy_n = (w + vlen < out_cap) ? vlen : (out_cap - 1 - w);
                        memcpy(out + w, val, copy_n);
                        w += copy_n;
                    }
                    i = (size_t)((close + 2) - tmpl);
                    continue;
                }
            }
        }

        if (w + 1 >= out_cap) {
            break;
        }
        out[w++] = tmpl[i++];
    }

    out[w] = '\0';
    return w;
}

size_t cjang_web_build_response(
    const char *method,
    int status_code,
    const char *reason,
    const char *content_type,
    const char *body,
    char *out,
    size_t out_cap
) {
    if (!out || out_cap == 0) {
        return 0;
    }
    const char *rsn = (reason && reason[0]) ? reason : "OK";
    const char *ct = (content_type && content_type[0]) ? content_type : "text/plain; charset=utf-8";
    const char *bd = body ? body : "";
    size_t blen = strlen(bd);
    int is_head = ascii_str_eq_ci(method, "HEAD");

    int n = snprintf(
        out,
        out_cap,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        rsn,
        ct,
        blen,
        is_head ? "" : bd
    );
    if (n < 0) {
        out[0] = '\0';
        return 0;
    }
    size_t written = (size_t)n;
    return written < out_cap ? written : (out_cap - 1);
}
