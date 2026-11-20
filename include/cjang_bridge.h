#ifndef CJANG_BRIDGE_H
#define CJANG_BRIDGE_H

#include "cjang_guard.h"
#include "cjang_tour.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CJANG_BRIDGE_OK = 0,
    CJANG_BRIDGE_ERR_ARGS = 1,
    CJANG_BRIDGE_ERR_GUARD = 2,
    CJANG_BRIDGE_ERR_TOUR = 3
} cjang_bridge_err_t;

/*
 * Inspects `raw` for `key` via cjang_guard_trace_kv, writes the trace line into
 * `diag_out`, and renders matching tour stops via cjang_tour_render_report
 * using either the extracted value as prefix (when non-empty) or `fallback_prefix`
 * (when non-empty). Always writes NUL-terminated strings when capacity > 0.
 */
cjang_bridge_err_t cjang_bridge_inspect_and_report(
    const char *raw,
    const char *key,
    const cjang_tour_stop_t *stops,
    size_t stop_count,
    const char *fallback_prefix,
    char *diag_out,
    size_t diag_cap,
    char *report_out,
    size_t report_cap,
    size_t *matched_out
);

/*
 * Inspects `raw` for `key` via cjang_guard_trace_kv and tries candidate
 * prefixes in order: extracted value (if guard succeeded and non-empty), then
 * each non-empty entry in `candidates[0 .. candidate_count - 1]`. Renders the
 * report for the first candidate prefix that matches one or more stops.
 */
cjang_bridge_err_t cjang_bridge_inspect_multi_prefix(
    const char *raw,
    const char *key,
    const cjang_tour_stop_t *stops,
    size_t stop_count,
    const char *const *candidates,
    size_t candidate_count,
    char *diag_out,
    size_t diag_cap,
    char *report_out,
    size_t report_cap,
    size_t *matched_out
);

#ifdef __cplusplus
}
#endif

#endif /* CJANG_BRIDGE_H */
