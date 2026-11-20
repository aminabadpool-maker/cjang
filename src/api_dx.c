#include "api_dx.h"

#include <stdio.h>
#include <string.h>

static const char *dx_reason_phrase(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 302: return "Found";
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

int cjang_dx_req_inspect(
    const char *raw_req,
    size_t body_len,
    cjang_dx_req_info_t *out_info
) {
    if (!raw_req || !out_info) {
        return -1;
    }
    memset(out_info, 0, sizeof(*out_info));

    cjang_req_line_t rl;
    if (cjang_parse_request_line(raw_req, &rl) != 0) {
        return -1;
    }

    snprintf(out_info->method, sizeof(out_info->method), "%s", rl.method);
    snprintf(out_info->path, sizeof(out_info->path), "%s", rl.path);
    snprintf(out_info->query, sizeof(out_info->query), "%s", rl.query);
    out_info->body_len = body_len;
    return 0;
}

int cjang_dx_cookie_get(
    const char *raw_req,
    const char *cookie_name,
    char *out,
    size_t out_cap
) {
    if (!raw_req || !cookie_name || !cookie_name[0] || !out || out_cap == 0) {
        return -1;
    }

    char cookie_hdr[512];
    if (cjang_ext_header_get(raw_req, "cookie", cookie_hdr, sizeof(cookie_hdr)) <= 0) {
        return -1;
    }

    size_t nlen = strlen(cookie_name);
    const char *p = cookie_hdr;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == ';') {
            p++;
        }
        if (*p == '\0') {
            break;
        }

        const char *seg_end = strchr(p, ';');
        size_t seg_len = seg_end ? (size_t)(seg_end - p) : strlen(p);

        const char *eq = memchr(p, '=', seg_len);
        if (eq != NULL) {
            size_t klen = (size_t)(eq - p);
            if (klen == nlen && strncmp(p, cookie_name, nlen) == 0) {
                const char *vstart = eq + 1;
                size_t vlen = (size_t)((p + seg_len) - vstart);
                size_t copy_n = vlen < (out_cap - 1) ? vlen : (out_cap - 1);
                memcpy(out, vstart, copy_n);
                out[copy_n] = '\0';
                return (int)copy_n;
            }
        }

        if (!seg_end) {
            break;
        }
        p = seg_end + 1;
    }

    return -1;
}

int cjang_dx_query_get_decoded(
    const char *raw_req,
    const char *key,
    char *out,
    size_t out_cap
) {
    if (!raw_req || !key || !key[0] || !out || out_cap == 0) {
        return -1;
    }

    cjang_req_line_t rl;
    if (cjang_parse_request_line(raw_req, &rl) != 0 || rl.query[0] == '\0') {
        return -1;
    }

    size_t qlen = strlen(rl.query);
    size_t klen = strlen(key);
    size_t i = 0;

    while (i < qlen) {
        size_t amp = i;
        while (amp < qlen && rl.query[amp] != '&') {
            amp++;
        }
        size_t eq = i;
        while (eq < amp && rl.query[eq] != '=') {
            eq++;
        }
        if (eq < amp && (eq - i) == klen && strncmp(rl.query + i, key, klen) == 0) {
            const char *raw_val = rl.query + eq + 1;
            size_t raw_len = amp - (eq + 1);
            return (int)cjang_url_decode(raw_val, raw_len, out, out_cap);
        }
        i = (amp < qlen) ? (amp + 1) : qlen;
    }

    return -1;
}

size_t cjang_dx_json_response(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *json_body
) {
    if (!dst || dst_cap == 0) {
        return 0;
    }
    const char *body = (json_body && json_body[0]) ? json_body : "{}";
    size_t blen = strlen(body);
    const char *reason = dx_reason_phrase(status_code);

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

size_t cjang_dx_redirect_302(
    char *dst,
    size_t dst_cap,
    const char *location_url
) {
    if (!dst || dst_cap == 0) {
        return 0;
    }
    const char *loc = (location_url && location_url[0]) ? location_url : "/";
    const char *body = "redirecting\n";
    int n = snprintf(
        dst,
        dst_cap,
        "HTTP/1.1 302 Found\r\n"
        "Location: %s\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        loc,
        strlen(body),
        body
    );
    if (n < 0) {
        dst[0] = '\0';
        return 0;
    }
    size_t w = (size_t)n;
    return w < dst_cap ? w : (dst_cap - 1);
}
