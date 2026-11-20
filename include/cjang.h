#ifndef CJANG_H
#define CJANG_H

#include <stddef.h>

typedef struct {
    const char *method;
    const char *path;
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

cjang_response_t cjang_dispatch(const cjang_app_t *app, const cjang_request_t *req);
int cjang_serve(const cjang_app_t *app, const char *host, int port);

#endif
