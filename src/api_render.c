#include "api_render.h"

#include <stdio.h>
#include <string.h>

static const char *ctx_lookup(const cjang_api_ctx_t *ctx, const char *k, size_t klen) {
    if (!ctx || !ctx->items || !k || klen == 0) {
        return NULL;
    }
    for (size_t i = ctx->count; i > 0; i--) {
        const cjang_api_kv_t *it = &ctx->items[i - 1];
        if (it->key && strlen(it->key) == klen && strncmp(it->key, k, klen) == 0) {
            return it->val ? it->val : "";
        }
    }
    return NULL;
}

size_t cjang_api_render_text(
    char *out,
    size_t cap,
    const char *tpl,
    const cjang_api_ctx_t *ctx
) {
    if (!out || cap == 0) {
        return 0;
    }
    out[0] = '\0';
    if (!tpl) {
        return 0;
    }

    size_t w = 0;
    size_t i = 0;
    const size_t max_w = cap - 1;

    while (tpl[i] != '\0' && w < max_w) {
        if (tpl[i] == '{' && tpl[i + 1] == '{') {
            const char *kstart = tpl + i + 2;
            const char *kend = strstr(kstart, "}}");
            if (kend != NULL) {
                size_t klen = (size_t)(kend - kstart);
                const char *val = ctx_lookup(ctx, kstart, klen);
                const char *rep = val ? val : "";
                size_t rlen = strlen(rep);
                for (size_t j = 0; j < rlen && w < max_w; j++) {
                    out[w++] = rep[j];
                }
                i = (size_t)((kend + 2) - tpl);
                continue;
            }
        }
        out[w++] = tpl[i++];
    }

    out[w] = '\0';
    return w;
}

size_t cjang_api_build_http_response(
    char *resp_buf,
    size_t resp_cap,
    int status_code,
    const char *status_text,
    const char *content_type,
    const char *body
) {
    if (!resp_buf || resp_cap == 0 || !status_text || !content_type || !body) {
        return 0;
    }

    size_t body_len = strlen(body);
    int n = snprintf(
        resp_buf,
        resp_cap,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        status_text,
        content_type,
        body_len,
        body
    );

    if (n < 0 || (size_t)n >= resp_cap) {
        return 0;
    }
    return (size_t)n;
}

size_t cjang_api_handle_render_request(
    const char *tpl,
    const cjang_api_ctx_t *ctx,
    char *http_out,
    size_t http_cap
) {
    if (!http_out || http_cap == 0) {
        return 0;
    }
    if (!tpl) {
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            400,
            "Bad Request",
            "text/plain; charset=utf-8",
            "missing template"
        );
    }

    char rendered[CJANG_API_BUF_CAP];
    cjang_api_render_text(rendered, sizeof(rendered), tpl, ctx);

    return cjang_api_build_http_response(
        http_out,
        http_cap,
        200,
        "OK",
        "text/plain; charset=utf-8",
        rendered
    );
}
