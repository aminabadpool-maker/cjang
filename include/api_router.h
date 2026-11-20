#ifndef CJANG_API_ROUTER_H
#define CJANG_API_ROUTER_H

#include "api_json.h"
#include <stddef.h>

#ifndef CJANG_PATH_CAP
#define CJANG_PATH_CAP 256
#endif

#ifndef CJANG_METHOD_CAP
#define CJANG_METHOD_CAP 16
#endif

#ifndef CJANG_PARAM_CAP
#define CJANG_PARAM_CAP 64
#endif

typedef struct {
    char method[CJANG_METHOD_CAP];
    char path[CJANG_PATH_CAP];
    char query[CJANG_PATH_CAP];
} cjang_req_line_t;

typedef struct {
    const char *method;
    const char *pattern; /* supports exact path or single trailing '/<param>' */
    const char *param_key;
    const char *tpl;
} cjang_route_t;

int cjang_parse_request_line(const char *raw_req, cjang_req_line_t *out);

size_t cjang_parse_query_pairs(const char *query, cjang_post_kv_store_t *store);

/* Returns: 1 = matched, 0 = path not found, -1 = method not allowed */
int cjang_match_route(
    const cjang_route_t *routes,
    size_t route_count,
    const char *method,
    const char *path,
    const char **out_tpl,
    char *out_param_key,
    size_t param_key_cap,
    char *out_param_val,
    size_t param_val_cap
);

size_t cjang_dispatch_request(
    const cjang_route_t *routes,
    size_t route_count,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
);

int cjang_write_all_and_close(int fd, const char *buf, size_t len);

#endif
