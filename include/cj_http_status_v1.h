#ifndef CJ_HTTP_STATUS_V1_H
#define CJ_HTTP_STATUS_V1_H

#include <stddef.h>

size_t cj_http_build_400(char *resp_buf, size_t resp_cap);
size_t cj_http_build_405(char *resp_buf, size_t resp_cap);

#endif
