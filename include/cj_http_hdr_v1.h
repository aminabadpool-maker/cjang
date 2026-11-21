#ifndef CJ_HTTP_HDR_V1_H
#define CJ_HTTP_HDR_V1_H

#include <stddef.h>

int cj_http_get_header(const char *req_buf, const char *name, char *val_out, size_t val_cap);

#endif
