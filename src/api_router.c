#include "api_router.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void copy_n_term(char *dst, size_t cap, const char *src, size_t len) {
    if (!dst || cap == 0) {
        return;
    }
    size_t n = len < (cap - 1) ? len : (cap - 1);
    if (src && n > 0) {
        memcpy(dst, src, n);
    }
    dst[n] = '\0';
}

int cjang_parse_request_line(const char *raw_req, cjang_req_line_t *out) {
    if (!raw_req || !out) {
        return -1;
    }
    out->method[0] = '\0';
    out->path[0] = '\0';
    out->query[0] = '\0';

    const char *line_end = strstr(raw_req, "\r\n");
    size_t line_len = line_end ? (size_t)(line_end - raw_req) : strlen(raw_req);
    if (line_len == 0) {
        return -1;
    }

    const char *sp1 = memchr(raw_req, ' ', line_len);
    if (!sp1 || sp1 == raw_req) {
        return -1;
    }
    size_t mlen = (size_t)(sp1 - raw_req);
    if (mlen >= sizeof(out->method)) {
        return -1;
    }
    copy_n_term(out->method, sizeof(out->method), raw_req, mlen);

    const char *target_start = sp1 + 1;
    size_t rem = line_len - mlen - 1;
    const char *sp2 = memchr(target_start, ' ', rem);
    size_t tlen = sp2 ? (size_t)(sp2 - target_start) : rem;
    if (tlen == 0 || target_start[0] != '/') {
        return -1;
    }

    const char *qmark = memchr(target_start, '?', tlen);
    if (qmark) {
        size_t plen = (size_t)(qmark - target_start);
        size_t qlen = (target_start + tlen) - (qmark + 1);
        copy_n_term(out->path, sizeof(out->path), target_start, plen);
        copy_n_term(out->query, sizeof(out->query), qmark + 1, qlen);
    } else {
        copy_n_term(out->path, sizeof(out->path), target_start, tlen);
    }

    return 0;
}

size_t cjang_parse_query_pairs(const char *query, cjang_post_kv_store_t *store) {
    if (!store) {
        return 0;
    }
    if (!query || query[0] == '\0') {
        store->count = 0;
        return 0;
    }
    return cjang_parse_form_pairs(query, strlen(query), store);
}

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
) {
    if (!routes || !method || !path || !out_tpl) {
        return 0;
    }
    if (out_param_key && param_key_cap > 0) {
        out_param_key[0] = '\0';
    }
    if (out_param_val && param_val_cap > 0) {
        out_param_val[0] = '\0';
    }

    int path_matched_wrong_method = 0;

    for (size_t i = 0; i < route_count; i++) {
        const cjang_route_t *r = &routes[i];
        if (!r->pattern || !r->tpl || !r->method) {
            continue;
        }

        const char *wild = strstr(r->pattern, "/<");
        int matched_path = 0;

        if (!wild) {
            if (strcmp(r->pattern, path) == 0) {
                matched_path = 1;
            }
        } else {
            size_t prefix_len = (size_t)(wild - r->pattern);
            if (strncmp(r->pattern, path, prefix_len) == 0 && path[prefix_len] == '/') {
                const char *val_start = path + prefix_len + 1;
                if (*val_start != '\0' && strchr(val_start, '/') == NULL) {
                    matched_path = 1;
                    if (matched_path && strcmp(r->method, method) == 0) {
                        if (r->param_key && out_param_key && param_key_cap > 0) {
                            copy_n_term(out_param_key, param_key_cap, r->param_key, strlen(r->param_key));
                        }
                        if (out_param_val && param_val_cap > 0) {
                            copy_n_term(out_param_val, param_val_cap, val_start, strlen(val_start));
                        }
                    }
                }
            }
        }

        if (matched_path) {
            if (strcmp(r->method, method) == 0) {
                *out_tpl = r->tpl;
                return 1;
            }
            path_matched_wrong_method = 1;
        }
    }

    return path_matched_wrong_method ? -1 : 0;
}

size_t cjang_dispatch_request(
    const cjang_route_t *routes,
    size_t route_count,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
) {
    if (!http_out || http_cap == 0) {
        return 0;
    }

    cjang_req_line_t req;
    if (cjang_parse_request_line(raw_req, &req) != 0) {
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            400,
            "Bad Request",
            "text/plain; charset=utf-8",
            "bad request line"
        );
    }

    const char *tpl = NULL;
    char pkey[CJANG_PARAM_CAP];
    char pval[CJANG_PARAM_CAP];
    int mres = cjang_match_route(
        routes,
        route_count,
        req.method,
        req.path,
        &tpl,
        pkey,
        sizeof(pkey),
        pval,
        sizeof(pval)
    );

    if (mres == 0) {
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            404,
            "Not Found",
            "text/plain; charset=utf-8",
            "not found"
        );
    }
    if (mres < 0) {
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            405,
            "Method Not Allowed",
            "text/plain; charset=utf-8",
            "method not allowed"
        );
    }

    cjang_post_kv_store_t store;
    store.count = 0;

    if (body && body_len > 0) {
        cjang_parse_flat_json(body, body_len, &store);
        if (store.count == 0) {
            cjang_parse_form_pairs(body, body_len, &store);
        }
    } else if (req.query[0] != '\0') {
        cjang_parse_query_pairs(req.query, &store);
    }

    if (pkey[0] != '\0' && store.count < CJANG_POST_MAX_KV) {
        copy_n_term(store.slots[store.count].key, sizeof(store.slots[store.count].key), pkey, strlen(pkey));
        copy_n_term(store.slots[store.count].val, sizeof(store.slots[store.count].val), pval, strlen(pval));
        store.count++;
    }

    cjang_api_ctx_t ctx;
    cjang_post_store_to_ctx(&store, &ctx);
    return cjang_api_handle_render_request(tpl, &ctx, http_out, http_cap);
}

int cjang_write_all_and_close(int fd, const char *buf, size_t len) {
    int rc = 0;
    if (fd < 0) {
        return -1;
    }
    if (buf && len > 0) {
        size_t sent = 0;
        while (sent < len) {
            ssize_t w = write(fd, buf + sent, len - sent);
            if (w <= 0) {
                rc = -1;
                break;
            }
            sent += (size_t)w;
        }
    }
    if (close(fd) != 0 && rc == 0) {
        rc = -1;
    }
    return rc;
}
