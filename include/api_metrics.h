#ifndef CJANG_API_METRICS_H
#define CJANG_API_METRICS_H

#include "api_router.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t total_requests;
    uint64_t ok_200;
    uint64_t client_4xx;
    uint64_t server_5xx;
    uint64_t total_resp_bytes;
    uint64_t last_exec_us;
    uint64_t max_exec_us;
} cjang_metrics_t;

void cjang_metrics_reset(cjang_metrics_t *m);

/* Dispatches request, records elapsed microseconds, updates metrics,
 * and injects X-Cjang-Us and X-Cjang-Bytes headers into the HTTP response. */
size_t cjang_dispatch_metered(
    cjang_metrics_t *m,
    const cjang_route_t *routes,
    size_t route_count,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
);

#endif
