#ifndef CJANG_H
#define CJANG_H

#include <stddef.h>

#define CJANG_MAX_PARAMS 8
#define CJANG_PARAM_KEY_LEN 32
#define CJANG_PARAM_VAL_LEN 128

#define CJANG_MAX_QUERY_PARAMS 16
#define CJANG_QUERY_KEY_LEN 32
#define CJANG_QUERY_VAL_LEN 128

#define CJANG_MAX_HEADERS 24
#define CJANG_HEADER_NAME_LEN 48
#define CJANG_HEADER_VAL_LEN 192

#define CJANG_MAX_BODY_LEN 2048

typedef struct {
    char key[CJANG_PARAM_KEY_LEN];
    char value[CJANG_PARAM_VAL_LEN];
} cjang_param_t;

typedef struct {
    char key[CJANG_QUERY_KEY_LEN];
    char value[CJANG_QUERY_VAL_LEN];
} cjang_query_param_t;

typedef struct {
    char name[CJANG_HEADER_NAME_LEN];
    char value[CJANG_HEADER_VAL_LEN];
} cjang_header_t;

typedef struct {
    const char *method;
    const char *path;
    cjang_param_t params[CJANG_MAX_PARAMS];
    size_t param_count;
    cjang_query_param_t query[CJANG_MAX_QUERY_PARAMS];
    size_t query_count;
    cjang_header_t headers[CJANG_MAX_HEADERS];
    size_t header_count;
    char body[CJANG_MAX_BODY_LEN];
    size_t body_len;
} cjang_request_t;

typedef struct {
    int status;
    const char *content_type;
    const char *body;
} cjang_response_t;

typedef cjang_response_t (*cjang_view_fn)(const cjang_request_t *req);

typedef struct {
    const char *method;
    const char *path;
    cjang_view_fn view;
} cjang_route_t;

typedef int (*cjang_middleware_fn)(const cjang_request_t *req);

typedef struct {
    const cjang_route_t *routes;
    size_t route_count;
    const cjang_middleware_fn *middlewares;
    size_t middleware_count;
} cjang_app_t;

const char *cjang_param_get(const cjang_request_t *req, const char *key);
const char *cjang_query_get(const cjang_request_t *req, const char *key);
const char *cjang_header_get(const cjang_request_t *req, const char *name);
void cjang_parse_query(cjang_request_t *req, const char *raw_query);
cjang_response_t cjang_dispatch(const cjang_app_t *app, cjang_request_t *req);
int cjang_serve(const cjang_app_t *app, const char *host, int port);


int cjang_form_get(const cjang_request_t *req, const char *key, char *out, size_t out_sz);
int cjang_json_get_str(const cjang_request_t *req, const char *key, char *out, size_t out_sz);

#define CJANG_MAX_CTX_VARS 16
#define CJANG_CTX_KEY_LEN 32
#define CJANG_CTX_VAL_LEN 128

typedef struct {
    char key[CJANG_CTX_KEY_LEN];
    char value[CJANG_CTX_VAL_LEN];
} cjang_ctx_var_t;

typedef struct {
    cjang_ctx_var_t vars[CJANG_MAX_CTX_VARS];
    size_t count;
} cjang_ctx_t;

void cjang_ctx_init(cjang_ctx_t *ctx);
int cjang_ctx_set(cjang_ctx_t *ctx, const char *key, const char *value);
const char *cjang_ctx_get(const cjang_ctx_t *ctx, const char *key);
int cjang_render_template(const char *tmpl, const cjang_ctx_t *ctx, char *out, size_t out_sz);
#endif
