#include "cjang_bridge.h"

#include <string.h>

static void clear_buf(char *buf, size_t cap) {
    if (buf != NULL && cap > 0u) {
        buf[0] = '\0';
    }
}

static int try_render_prefix(
    const char *prefix,
    const cjang_tour_stop_t *stops,
    size_t stop_count,
    char *report_out,
    size_t report_cap,
    size_t *matched_out
) {
    if (prefix == NULL || prefix[0] == '\0') {
        return 0;
    }

    size_t matched = 0u;
    int trc = cjang_tour_render_report(
        stops,
        stop_count,
        prefix,
        report_out,
        report_cap,
        &matched
    );
    if (trc == CJANG_TOUR_OK && matched > 0u) {
        if (matched_out != NULL) {
            *matched_out = matched;
        }
        return 1;
    }

    clear_buf(report_out, report_cap);
    if (matched_out != NULL) {
        *matched_out = 0u;
    }
    return 0;
}

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
) {
    const char *candidates[1];
    size_t cand_count = 0u;
    if (fallback_prefix != NULL && fallback_prefix[0] != '\0') {
        candidates[0] = fallback_prefix;
        cand_count = 1u;
    }

    return cjang_bridge_inspect_multi_prefix(
        raw,
        key,
        stops,
        stop_count,
        candidates,
        cand_count,
        diag_out,
        diag_cap,
        report_out,
        report_cap,
        matched_out
    );
}

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
) {
    if (matched_out != NULL) {
        *matched_out = 0u;
    }
    clear_buf(diag_out, diag_cap);
    clear_buf(report_out, report_cap);

    if (stops == NULL || stop_count == 0u || report_out == NULL || report_cap == 0u) {
        return CJANG_BRIDGE_ERR_ARGS;
    }
    if (candidate_count > 0u && candidates == NULL) {
        return CJANG_BRIDGE_ERR_ARGS;
    }

    char extracted_val[128];
    extracted_val[0] = '\0';

    cjang_guard_err_t grc = cjang_guard_trace_kv(
        raw,
        key,
        extracted_val,
        sizeof(extracted_val),
        diag_out,
        diag_cap
    );

    if (grc == CJANG_GUARD_OK && extracted_val[0] != '\0') {
        if (try_render_prefix(
                extracted_val,
                stops,
                stop_count,
                report_out,
                report_cap,
                matched_out)) {
            return CJANG_BRIDGE_OK;
        }
    }

    for (size_t i = 0u; i < candidate_count; i++) {
        if (try_render_prefix(
                candidates[i],
                stops,
                stop_count,
                report_out,
                report_cap,
                matched_out)) {
            return (grc == CJANG_GUARD_OK) ? CJANG_BRIDGE_OK : CJANG_BRIDGE_ERR_GUARD;
        }
    }

    if (grc != CJANG_GUARD_OK) {
        return CJANG_BRIDGE_ERR_GUARD;
    }

    return CJANG_BRIDGE_ERR_TOUR;
}
