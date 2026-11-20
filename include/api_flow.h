#ifndef CJANG_API_FLOW_H
#define CJANG_API_FLOW_H

#include "api_dx.h"
#include "api_plus.h"
#include "api_router.h"
#include "api_web.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char method[16];
    char path[CJANG_PATH_CAP];
    char query[CJANG_PATH_CAP];
    char bearer[128];
    int has_bearer;
    size_t body_len;
    int route_idx;      /* >= 0 if matched, -1 if 404, -2 if 405 */
    int status_hint;    /* 200, 400, 404, 405, 413 */
    char cookie_val[128];
    int has_cookie;
    uint64_t prep_us;
} cjang_flow_ctx_t;

/* Prepares request context and, if status_hint != 200, writes ready HTTP response into resp_out.
 * Returns 1 if ready to run handler, or 0 if error/404/405 response was already written. */
int cjang_flow_prepare(
    const cjang_route_t *routes,
    size_t route_count,
    const char *allow_list,
    const char *raw_req,
    size_t body_len,
    size_t max_body_len,
    cjang_flow_ctx_t *ctx,
    char *resp_out,
    size_t resp_cap
);

/* Builds standard HTTP/1.1 JSON response with status code and JSON body. Returns byte length. */
size_t cjang_flow_json(
    char *resp_out,
    size_t resp_cap,
    int status_code,
    const char *json_body
);

/* Builds standard HTTP/1.1 plain text response with status code. Returns byte length. */
size_t cjang_flow_text(
    char *resp_out,
    size_t resp_cap,
    int status_code,
    const char *text_body
);

/* Formats a clean single-line request summary into log_out. Returns written length. */
size_t cjang_flow_log_line(
    const cjang_flow_ctx_t *ctx,
    int status_code,
    size_t resp_bytes,
    char *log_out,
    size_t log_cap
);

/* Extended flow preparation with optional cookie extraction and timing (us). */
int cjang_flow_prepare_v2(
    const cjang_route_t *routes,
    size_t route_count,
    const char *allow_list,
    const char *raw_req,
    const char *cookie_name,
    size_t body_len,
    size_t max_body_len,
    cjang_flow_ctx_t *ctx,
    char *resp_out,
    size_t resp_cap
);

#endif
