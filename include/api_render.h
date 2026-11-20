#ifndef CJANG_API_RENDER_H
#define CJANG_API_RENDER_H

#include <stddef.h>

#ifndef CJANG_API_BUF_CAP
#define CJANG_API_BUF_CAP 2048
#endif

typedef struct {
    const char *key;
    const char *val;
} cjang_api_kv_t;

typedef struct {
    const cjang_api_kv_t *items;
    size_t count;
} cjang_api_ctx_t;

size_t cjang_api_render_text(
    char *out,
    size_t cap,
    const char *tpl,
    const cjang_api_ctx_t *ctx
);

size_t cjang_api_build_http_response(
    char *resp_buf,
    size_t resp_cap,
    int status_code,
    const char *status_text,
    const char *content_type,
    const char *body
);

size_t cjang_api_handle_render_request(
    const char *tpl,
    const cjang_api_ctx_t *ctx,
    char *http_out,
    size_t http_cap
);

#endif
