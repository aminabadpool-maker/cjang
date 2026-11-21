#ifndef CJ_HTTP_STATIC_V1_H
#define CJ_HTTP_STATIC_V1_H

#include <stddef.h>

#ifndef CJ_HTTP_RESP_MAX
#define CJ_HTTP_RESP_MAX 70000u
#endif

int cj_http_build_static_200(const char *root_dir, const char *relpath, char *resp_buf, size_t resp_cap, size_t *out_len);
size_t cj_http_build_404(char *resp_buf, size_t resp_cap);

#endif
