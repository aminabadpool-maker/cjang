#ifndef CJ_HTTP_DISPATCH_V1_H
#define CJ_HTTP_DISPATCH_V1_H

#include <stddef.h>

size_t cj_http_dispatch_static(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
);

#endif
