#include "cjang_bridge.h"

#include <string.h>

static void clear_buf(char *buf, size_t cap) {
    if (buf != NULL && cap > 0u) {
        buf[0] = '\0';
    }
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
    if (matched_out != NULL) {
        *matched_out = 0u;
    }
    clear_buf(diag_out, diag_cap);
    clear_buf(report_out, report_cap);

    if (stops == NULL || stop_count == 0u || report_out == NULL || report_cap == 0u) {
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

    const char *active_prefix = NULL;
    if (grc == CJANG_GUARD_OK && extracted_val[0] != '\0') {
        active_prefix = extracted_val;
    } else if (fallback_prefix != NULL && fallback_prefix[0] != '\0') {
        active_prefix = fallback_prefix;
    } else {
        return CJANG_BRIDGE_ERR_GUARD;
    }

    int trc = cjang_tour_render_report(
        stops,
        stop_count,
        active_prefix,
        report_out,
        report_cap,
        matched_out
    );
    if (trc != CJANG_TOUR_OK) {
        clear_buf(report_out, report_cap);
        if (matched_out != NULL) {
            *matched_out = 0u;
        }
        return CJANG_BRIDGE_ERR_TOUR;
    }

    if (grc != CJANG_GUARD_OK) {
        return CJANG_BRIDGE_ERR_GUARD;
    }

    return CJANG_BRIDGE_OK;
}
