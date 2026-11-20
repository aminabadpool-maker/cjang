#include "cjang.h"
#include <stdio.h>
#include <string.h>

const char *cjang_param_get(const cjang_request_t *req, const char *key) {
    if (!req || !key) return NULL;
    for (size_t i = 0; i < req->param_count; ++i) {
        if (strcmp(req->params[i].key, key) == 0) {
            return req->params[i].value;
        }
    }
    return NULL;
}

const char *cjang_query_get(const cjang_request_t *req, const char *key) {
    if (!req || !key) return NULL;
    for (size_t i = 0; i < req->query_count; ++i) {
        if (strcmp(req->query[i].key, key) == 0) {
            return req->query[i].value;
        }
    }
    return NULL;
}

void cjang_parse_query(cjang_request_t *req, const char *raw_query) {
    if (!req) return;
    req->query_count = 0;
    if (!raw_query || *raw_query == '\0') return;

    const char *p = raw_query;
    while (*p && req->query_count < CJANG_MAX_QUERY_PARAMS) {
        const char *amp = strchr(p, '&');
        size_t pair_len = amp ? (size_t)(amp - p) : strlen(p);

        if (pair_len > 0) {
            const char *eq = memchr(p, '=', pair_len);
            size_t k_len = eq ? (size_t)(eq - p) : pair_len;
            size_t v_len = eq ? (pair_len - k_len - 1) : 0;
            const char *v_ptr = eq ? (eq + 1) : "";

            if (k_len > 0 && k_len < CJANG_QUERY_KEY_LEN && v_len < CJANG_QUERY_VAL_LEN) {
                cjang_query_param_t *qp = &req->query[req->query_count];
                memcpy(qp->key, p, k_len);
                qp->key[k_len] = '\0';
                memcpy(qp->value, v_ptr, v_len);
                qp->value[v_len] = '\0';
                req->query_count++;
            }
        }

        if (!amp) break;
        p = amp + 1;
    }
}

static int store_param(cjang_request_t *req, const char *name, size_t name_len,
                       const char *val, size_t val_len) {
    if (req->param_count >= CJANG_MAX_PARAMS) return 0;
    if (name_len == 0 || val_len == 0) return 0;
    if (name_len >= CJANG_PARAM_KEY_LEN || val_len >= CJANG_PARAM_VAL_LEN) return 0;

    cjang_param_t *p = &req->params[req->param_count];
    memcpy(p->key, name, name_len);
    p->key[name_len] = '\0';
    memcpy(p->value, val, val_len);
    p->value[val_len] = '\0';
    req->param_count++;
    return 1;
}

static int match_pattern(const char *pattern, const char *path, cjang_request_t *req) {
    req->param_count = 0;
    const char *pp = pattern;
    const char *rp = path;

    while (*pp && *rp) {
        if (*pp == '<') {
            const char *close = strchr(pp + 1, '>');
            if (!close) return 0;
            size_t name_len = (size_t)(close - (pp + 1));

            const char *r_end = strchr(rp, '/');
            size_t val_len = r_end ? (size_t)(r_end - rp) : strlen(rp);

            if (!store_param(req, pp + 1, name_len, rp, val_len)) {
                req->param_count = 0;
                return 0;
            }

            pp = close + 1;
            rp += val_len;
        } else {
            if (*pp != *rp) {
                req->param_count = 0;
                return 0;
            }
            pp++;
            rp++;
        }
    }

    if (*pp == '\0' && *rp == '\0') {
        return 1;
    }

    req->param_count = 0;
    return 0;
}

cjang_response_t cjang_dispatch(const cjang_app_t *app, cjang_request_t *req) {
    if (!app || !req || !req->method || !req->path) {
        cjang_response_t bad = {400, "text/plain; charset=utf-8", "Bad Request\n"};
        return bad;
    }

    req->param_count = 0;

    for (size_t i = 0; i < app->middleware_count; ++i) {
        if (app->middlewares[i] && app->middlewares[i](req) != 0) {
            cjang_response_t blocked = {403, "text/plain; charset=utf-8", "Forbidden by middleware\n"};
            return blocked;
        }
    }

    for (size_t i = 0; i < app->route_count; ++i) {
        const cjang_route_t *r = &app->routes[i];
        if (strcmp(r->method, req->method) == 0 && match_pattern(r->path, req->path, req)) {
            return r->view(req);
        }
    }

    cjang_response_t not_found = {404, "text/plain; charset=utf-8", "Not Found\n"};
    return not_found;
}
