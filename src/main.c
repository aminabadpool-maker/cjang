#include "cjang.h"
#include <stdio.h>

static int log_middleware(const cjang_request_t *req) {
    printf("[middleware] %s %s\n", req->method, req->path);
    fflush(stdout);
    return 0;
}

static cjang_response_t index_view(const cjang_request_t *req) {
    (void)req;
    cjang_response_t res = {200, "text/plain; charset=utf-8", "Welcome to cjang (C11)\n"};
    return res;
}

static cjang_response_t health_view(const cjang_request_t *req) {
    (void)req;
    cjang_response_t res = {200, "text/plain; charset=utf-8", "OK\n"};
    return res;
}

int main(void) {
    static const cjang_middleware_fn middlewares[] = {
        log_middleware
    };

    static const cjang_route_t routes[] = {
        {"GET", "/", index_view},
        {"GET", "/health/", health_view}
    };

    cjang_app_t app = {
        .routes = routes,
        .route_count = sizeof(routes) / sizeof(routes[0]),
        .middlewares = middlewares,
        .middleware_count = sizeof(middlewares) / sizeof(middlewares[0])
    };

    return cjang_serve(&app, "127.0.0.1", 8080);
}
