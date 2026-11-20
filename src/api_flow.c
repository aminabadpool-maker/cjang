#include "api_flow.h"

#include <stdio.h>
#include <string.h>

static int flow_path_exists_any_method(
    const cjang_route_t *routes,
    size_t route_count,
    const char *path
) {
    if (!routes || route_count == 0 || !path) {
        return 0;
    }
    for (size_t i = 0; i < route_count; i++) {
        if (routes[i].pattern && strcmp(routes[i].pattern, path) == 0) {
            return 1;
        }
    }
    return 0;
}

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
) {
    if (!ctx || !raw_req) {
        return 0;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->route_idx = -1;
    ctx->status_hint = 400;
    ctx->body_len = body_len;

    cjang_req_line_t rl;
    if (cjang_parse_request_line(raw_req, &rl) != 0) {
        cjang_flow_json(resp_out, resp_cap, 400, "{\"error\":\"bad_request\"}");
        return 0;
    }

    snprintf(ctx->method, sizeof(ctx->method), "%s", rl.method);
    snprintf(ctx->path, sizeof(ctx->path), "%s", rl.path);
    snprintf(ctx->query, sizeof(ctx->query), "%s", rl.query);

    size_t parsed_clen = 0;
    if (cjang_web_parse_content_length(raw_req, max_body_len, &parsed_clen) != 0 ||
        body_len > max_body_len) {
        ctx->status_hint = 413;
        cjang_flow_json(resp_out, resp_cap, 413, "{\"error\":\"payload_too_large\"}");
        return 0;
    }
    if (parsed_clen > 0) {
        ctx->body_len = parsed_clen;
    }

    if (cjang_plus_bearer_get(raw_req, ctx->bearer, sizeof(ctx->bearer)) > 0) {
        ctx->has_bearer = 1;
    }

    int idx = cjang_plus_route_match_idx(routes, route_count, ctx->method, ctx->path);
    if (idx >= 0) {
        ctx->route_idx = idx;
        ctx->status_hint = 200;
        return 1;
    }

    if (flow_path_exists_any_method(routes, route_count, ctx->path)) {
        ctx->route_idx = -2;
        ctx->status_hint = 405;
        cjang_web_check_method_or_405(ctx->method, allow_list, resp_out, resp_cap);
        return 0;
    }

    ctx->route_idx = -1;
    ctx->status_hint = 404;
    cjang_flow_json(resp_out, resp_cap, 404, "{\"error\":\"not_found\"}");
    return 0;
}

size_t cjang_flow_json(
    char *resp_out,
    size_t resp_cap,
    int status_code,
    const char *json_body
) {
    return cjang_plus_build_json_resp(resp_out, resp_cap, status_code, json_body);
}

size_t cjang_flow_text(
    char *resp_out,
    size_t resp_cap,
    int status_code,
    const char *text_body
) {
    const char *reason = cjang_plus_status_reason(status_code);
    return cjang_web_build_response("GET", status_code, reason, "text/plain; charset=utf-8", text_body, resp_out, resp_cap);
}

size_t cjang_flow_log_line(
    const cjang_flow_ctx_t *ctx,
    int status_code,
    size_t resp_bytes,
    char *log_out,
    size_t log_cap
) {
    if (!log_out || log_cap == 0) {
        return 0;
    }
    const char *m = (ctx && ctx->method[0]) ? ctx->method : "?";
    const char *p = (ctx && ctx->path[0]) ? ctx->path : "/";
    int n = snprintf(log_out, log_cap, "%s %s -> %d (%zuB)", m, p, status_code, resp_bytes);
    if (n < 0) {
        log_out[0] = '\0';
        return 0;
    }
    size_t w = (size_t)n;
    return w < log_cap ? w : (log_cap - 1);
}

#define _POSIX_C_SOURCE 200809L
#include <time.h>

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
) {
    struct timespec t0;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    int ok = cjang_flow_prepare(
        routes,
        route_count,
        allow_list,
        raw_req,
        body_len,
        max_body_len,
        ctx,
        resp_out,
        resp_cap
    );

    if (ctx) {
        if (cookie_name && cookie_name[0] && raw_req) {
            if (cjang_dx_cookie_get(raw_req, cookie_name, ctx->cookie_val, sizeof(ctx->cookie_val)) >= 0) {
                ctx->has_cookie = 1;
            }
        }
        ctx->prep_us = cjang_plus_elapsed_us_since(&t0);
    }
    return ok;
}
