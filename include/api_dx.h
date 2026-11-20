#ifndef CJANG_API_DX_H
#define CJANG_API_DX_H

#include "api_codec.h"
#include "api_ext.h"
#include "api_router.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char method[16];
    char path[CJANG_PATH_CAP];
    char query[CJANG_PATH_CAP];
    size_t body_len;
} cjang_dx_req_info_t;

/* Parses request line + Content-Length into a compact request summary. Returns 0 on ok, -1 on err. */
int cjang_dx_req_inspect(
    const char *raw_req,
    size_t body_len,
    cjang_dx_req_info_t *out_info
);

/* Extracts cookie value by name from Cookie header in raw_req. Returns len (>=0) or -1. */
int cjang_dx_cookie_get(
    const char *raw_req,
    const char *cookie_name,
    char *out,
    size_t out_cap
);

/* Extracts and URL-decodes a query parameter from raw_req request line. Returns len (>=0) or -1. */
int cjang_dx_query_get_decoded(
    const char *raw_req,
    const char *key,
    char *out,
    size_t out_cap
);

/* Builds HTTP/1.1 JSON response (Django JsonResponse style). Returns written bytes. */
size_t cjang_dx_json_response(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *json_body
);

/* Builds HTTP/1.1 302 redirect response with Location header. Returns written bytes. */
size_t cjang_dx_redirect_302(
    char *dst,
    size_t dst_cap,
    const char *location_url
);

#endif
