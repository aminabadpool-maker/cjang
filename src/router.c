#include "cjang.h"
#include <string.h>

cjang_response_t cjang_dispatch(const cjang_app_t *app, const cjang_request_t *req) {
    if (!app || !req || !req->method || !req->path) {
        cjang_response_t bad = {400, "text/plain; charset=utf-8", "Bad Request\n"};
        return bad;
    }

    for (size_t i = 0; i < app->middleware_count; ++i) {
        if (app->middlewares[i] && app->middlewares[i](req) != 0) {
            cjang_response_t blocked = {403, "text/plain; charset=utf-8", "Forbidden by middleware\n"};
            return blocked;
        }
    }

    for (size_t i = 0; i < app->route_count; ++i) {
        const cjang_route_t *r = &app->routes[i];
        if (strcmp(r->method, req->method) == 0 && strcmp(r->path, req->path) == 0) {
            return r->view(req);
        }
    }

    cjang_response_t not_found = {404, "text/plain; charset=utf-8", "Not Found\n"};
    return not_found;
}
