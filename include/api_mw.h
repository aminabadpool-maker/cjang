#ifndef CJANG_API_MW_H
#define CJANG_API_MW_H

#include "api_metrics.h"
#include <stddef.h>
#include <stdint.h>

#ifndef CJANG_MW_MAX
#define CJANG_MW_MAX 8
#endif

typedef enum {
    CJANG_MW_PASS = 0,
    CJANG_MW_BLOCK = 1
} cjang_mw_verdict_t;

/* Middleware callback:
 * If it decides to block the request, it writes a full HTTP response into
 * block_out (up to block_cap) and returns CJANG_MW_BLOCK.
 * Otherwise it returns CJANG_MW_PASS. */
typedef cjang_mw_verdict_t (*cjang_mw_fn)(
    void *user_data,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *block_out,
    size_t block_cap
);

typedef struct {
    cjang_mw_fn fn;
    void *user_data;
} cjang_mw_entry_t;

typedef struct {
    cjang_mw_entry_t entries[CJANG_MW_MAX];
    size_t count;
} cjang_mw_chain_t;

typedef struct {
    const char *header_line_prefix; /* e.g. "X-Agent-Key: secret" */
    const char *deny_body;
} cjang_mw_hdr_guard_t;

void cjang_mw_init(cjang_mw_chain_t *chain);
int cjang_mw_add(cjang_mw_chain_t *chain, cjang_mw_fn fn, void *user_data);

/* Runs middleware chain first; if blocked, writes response and returns its length.
 * If all pass, delegates directly to cjang_dispatch_metered. */
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
);

/* Built-in lightweight middlewares */
cjang_mw_verdict_t cjang_mw_count_req(
    void *user_data,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *block_out,
    size_t block_cap
);

cjang_mw_verdict_t cjang_mw_require_header_line(
    void *user_data,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *block_out,
    size_t block_cap
);

#endif
