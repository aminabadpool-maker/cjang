#ifndef CJANG_API_JSON_H
#define CJANG_API_JSON_H

#include "api_post.h"
#include <stddef.h>

size_t cjang_parse_flat_json(
    const char *body,
    size_t body_len,
    cjang_post_kv_store_t *store
);

size_t cjang_api_handle_post_auto(
    const char *tpl,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
);

#endif
