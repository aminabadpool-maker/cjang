#include "cjang.h"
#include <stdio.h>
#include <string.h>

static char g_user_body[256];
static char g_echo_body[CJANG_MAX_BODY_LEN + 128];
static char g_submit_body[256];
static char g_greet_body[512];

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

static cjang_response_t user_detail_view(const cjang_request_t *req) {
    const char *id = cjang_param_get(req, "id");
    if (!id) {
        cjang_response_t bad = {400, "text/plain; charset=utf-8", "Missing id param\n"};
        return bad;
    }

    const char *page = cjang_query_get(req, "page");
    if (page) {
        snprintf(g_user_body, sizeof(g_user_body), "User ID: %s (page=%s)\n", id, page);
    } else {
        snprintf(g_user_body, sizeof(g_user_body), "User ID: %s\n", id);
    }

    cjang_response_t res = {200, "text/plain; charset=utf-8", g_user_body};
    return res;
}

static cjang_response_t echo_post_view(const cjang_request_t *req) {
    const char *ctype = cjang_header_get(req, "Content-Type");
    snprintf(
        g_echo_body,
        sizeof(g_echo_body),
        "Content-Type: %s\nBytes: %zu\nBody: %s\n",
        ctype ? ctype : "none",
        req->body_len,
        req->body
    );
    cjang_response_t res = {200, "text/plain; charset=utf-8", g_echo_body};
    return res;
}

static cjang_response_t submit_view(const cjang_request_t *req) {
    char name[64];
    char role[64];
    const char *ctype = cjang_header_get(req, "Content-Type");

    int ok_name = 0;
    int ok_role = 0;

    if (ctype && strstr(ctype, "application/json") != NULL) {
        ok_name = cjang_json_get_str(req, "name", name, sizeof(name));
        ok_role = cjang_json_get_str(req, "role", role, sizeof(role));
    } else {
        ok_name = cjang_form_get(req, "name", name, sizeof(name));
        ok_role = cjang_form_get(req, "role", role, sizeof(role));
    }

    snprintf(
        g_submit_body,
        sizeof(g_submit_body),
        "name=%s, role=%s\n",
        ok_name ? name : "missing",
        ok_role ? role : "missing"
    );

    cjang_response_t res = {200, "text/plain; charset=utf-8", g_submit_body};
    return res;
}

static cjang_response_t greet_view(const cjang_request_t *req) {
    const char *name = cjang_param_get(req, "name");
    const char *lang = cjang_query_get(req, "lang");

    cjang_ctx_t ctx;
    cjang_ctx_init(&ctx);
    cjang_ctx_set(&ctx, "name", name ? name : "guest");
    cjang_ctx_set(&ctx, "lang", lang ? lang : "C11");

    static const char tmpl[] = "Hello, {{name}}! Running on {{lang}}.\n";
    cjang_render_template(tmpl, &ctx, g_greet_body, sizeof(g_greet_body));

    cjang_response_t res = {200, "text/plain; charset=utf-8", g_greet_body};
    return res;
}

int main(void) {
    static const cjang_middleware_fn middlewares[] = {
        log_middleware
    };

    static const cjang_route_t routes[] = {
        {"GET", "/", index_view},
        {"GET", "/health/", health_view},
        {"GET", "/users/<id>/", user_detail_view},
        {"GET", "/greet/<name>/", greet_view},
        {"POST", "/echo/", echo_post_view},
        {"POST", "/submit/", submit_view}
    };

    cjang_app_t app = {
        .routes = routes,
        .route_count = sizeof(routes) / sizeof(routes[0]),
        .middlewares = middlewares,
        .middleware_count = sizeof(middlewares) / sizeof(middlewares[0])
    };

    return cjang_serve(&app, "127.0.0.1", 8080);
}
