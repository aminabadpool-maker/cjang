#include "api_mw.h"

#include <string.h>

static int has_line_prefix(const char *headers, const char *prefix) {
    if (!headers || !prefix || prefix[0] == '\0') {
        return 0;
    }
    size_t plen = strlen(prefix);
    const char *p = headers;

    while (*p != '\0') {
        if (strncmp(p, prefix, plen) == 0) {
            char after = p[plen];
            if (after == '\0' || after == '\r' || after == '\n') {
                return 1;
            }
        }
        const char *nl = strchr(p, '\n');
        if (!nl) {
            break;
        }
        p = nl + 1;
    }
    return 0;
}

void cjang_mw_init(cjang_mw_chain_t *chain) {
    if (!chain) {
        return;
    }
    memset(chain, 0, sizeof(*chain));
}

int cjang_mw_add(cjang_mw_chain_t *chain, cjang_mw_fn fn, void *user_data) {
    if (!chain || !fn || chain->count >= CJANG_MW_MAX) {
        return -1;
    }
    chain->entries[chain->count].fn = fn;
    chain->entries[chain->count].user_data = user_data;
    chain->count++;
    return 0;
}

size_t cjang_mw_dispatch_metered(
    const cjang_mw_chain_t *chain,
    cjang_metrics_t *m,
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

    if (chain != NULL) {
        for (size_t i = 0; i < chain->count; i++) {
            cjang_mw_fn fn = chain->entries[i].fn;
            if (!fn) {
                continue;
            }
            cjang_mw_verdict_t v = fn(
                chain->entries[i].user_data,
                raw_req,
                body,
                body_len,
                http_out,
                http_cap
            );
            if (v == CJANG_MW_BLOCK) {
                if (m != NULL) {
                    m->total_requests++;
                    m->client_4xx++;
                }
                return strlen(http_out);
            }
        }
    }

    return cjang_dispatch_metered(
        m,
        routes,
        route_count,
        raw_req,
        body,
        body_len,
        http_out,
        http_cap
    );
}

cjang_mw_verdict_t cjang_mw_count_req(
    void *user_data,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *block_out,
    size_t block_cap
) {
    (void)raw_req;
    (void)body;
    (void)body_len;
    (void)block_out;
    (void)block_cap;

    if (user_data != NULL) {
        uint64_t *cnt = (uint64_t *)user_data;
        (*cnt)++;
    }
    return CJANG_MW_PASS;
}

cjang_mw_verdict_t cjang_mw_require_header_line(
    void *user_data,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *block_out,
    size_t block_cap
) {
    (void)body;
    (void)body_len;

    const cjang_mw_hdr_guard_t *guard = (const cjang_mw_hdr_guard_t *)user_data;
    if (!guard || !guard->header_line_prefix) {
        return CJANG_MW_PASS;
    }

    if (has_line_prefix(raw_req, guard->header_line_prefix)) {
        return CJANG_MW_PASS;
    }

    const char *msg = (guard->deny_body && guard->deny_body[0] != '\0')
        ? guard->deny_body
        : "unauthorized\n";

    (void)cjang_api_build_http_response(
        block_out,
        block_cap,
        401,
        "Unauthorized",
        "text/plain; charset=utf-8",
        msg
    );
    return CJANG_MW_BLOCK;
}
