#ifndef CJ_HTTP_REQ_V1_H
#define CJ_HTTP_REQ_V1_H

#include <stddef.h>

#ifndef CJ_HTTP_METHOD_MAX
#define CJ_HTTP_METHOD_MAX 16u
#endif

#ifndef CJ_HTTP_TARGET_MAX
#define CJ_HTTP_TARGET_MAX 256u
#endif

int cj_http_parse_req_line(
    const char *req_buf,
    char *method_out, size_t method_cap,
    char *target_out, size_t target_cap
);

#endif
