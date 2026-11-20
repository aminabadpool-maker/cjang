#ifndef CJANG_API_POST_H
#define CJANG_API_POST_H

#include "api_render.h"
#include <stddef.h>

#ifndef CJANG_POST_MAX_KV
#define CJANG_POST_MAX_KV 16
#endif

#ifndef CJANG_POST_KEY_CAP
#define CJANG_POST_KEY_CAP 64
#endif

#ifndef CJANG_POST_VAL_CAP
#define CJANG_POST_VAL_CAP 256
#endif

typedef struct {
    char key[CJANG_POST_KEY_CAP];
    char val[CJANG_POST_VAL_CAP];
} cjang_post_slot_t;

typedef struct {
    cjang_post_slot_t slots[CJANG_POST_MAX_KV];
    cjang_api_kv_t view[CJANG_POST_MAX_KV];
    size_t count;
} cjang_post_kv_store_t;

int cjang_parse_content_length(const char *headers, size_t max_body_cap, size_t *out_len);

size_t cjang_parse_form_pairs(
    const char *body,
    size_t body_len,
    cjang_post_kv_store_t *store
);

void cjang_post_store_to_ctx(const cjang_post_kv_store_t *store, cjang_api_ctx_t *out_ctx);

size_t cjang_api_handle_post_render(
    const char *tpl,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
);

#endif
