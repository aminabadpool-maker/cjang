#ifndef CJ_HTTP_JSON_V1_H
#define CJ_HTTP_JSON_V1_H

#include <stddef.h>

#ifndef CJ_JSON_ESC_MAX
#define CJ_JSON_ESC_MAX 512u
#endif

size_t cj_json_escape(const char *src, char *dst, size_t dst_cap);
size_t cj_http_build_json_kv(
    int status_code,
    const char *status_text,
    const char *key,
    const char *val,
    char *out_buf,
    size_t out_cap
);

#endif
