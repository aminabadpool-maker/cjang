#ifndef CJ_HTTP_QUERY_V1_H
#define CJ_HTTP_QUERY_V1_H

#include <stddef.h>

int cj_http_get_query_param(
    const char *req_buf,
    const char *key,
    char *val_out,
    size_t val_cap
);

#endif
