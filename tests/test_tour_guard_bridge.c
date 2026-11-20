#include "cjang_bridge.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const cjang_tour_stop_t k_bridge_stops[] = {
    {"guard_kv",     "Guard KV Extractor",   "src/cjang_guard.c",  "Extract key/value safely"},
    {"guard_trace",  "Guard Trace Pipeline", "src/cjang_guard.c",  "Emit key/status/reason trace"},
    {"tour_prefix",  "Tour Prefix Filter",   "src/cjang_tour.c",   "Filter stops by key prefix"},
    {"tour_report",  "Tour Summary Report",  "src/cjang_tour.c",   "Render matched stops + summary"},
    {"bridge_route", "Bridge Route Helper",  "src/cjang_bridge.c", "Combine guard trace & tour report"}
};

int main(void) {
    char diag[256];
    char report[512];
    size_t matched = 0u;

    /* 1. Extracted prefix drives tour report directly */
    cjang_bridge_err_t rc1 = cjang_bridge_inspect_and_report(
        "{\"target\":\"guard_\",\"mode\":\"fast\"}",
        "target",
        k_bridge_stops,
        sizeof(k_bridge_stops) / sizeof(k_bridge_stops[0]),
        NULL,
        diag,
        sizeof(diag),
        report,
        sizeof(report),
        &matched
    );
    assert(rc1 == CJANG_BRIDGE_OK);
    assert(matched == 2u);
    assert(strstr(diag, "key=target") != NULL);
    assert(strstr(report, "guard_kv") != NULL);
    assert(strstr(report, "guard_trace") != NULL);

    /* 2. Missing key falls back cleanly to fallback_prefix and reports guard error */
    matched = 99u;
    cjang_bridge_err_t rc2 = cjang_bridge_inspect_and_report(
        "{\"mode\":\"fast\"}",
        "target",
        k_bridge_stops,
        sizeof(k_bridge_stops) / sizeof(k_bridge_stops[0]),
        "tour_",
        diag,
        sizeof(diag),
        report,
        sizeof(report),
        &matched
    );
    assert(rc2 == CJANG_BRIDGE_ERR_GUARD);
    assert(matched == 2u);
    assert(strstr(report, "tour_prefix") != NULL);
    assert(strstr(report, "tour_report") != NULL);

    /* 3. No fallback prefix and missing key returns ERR_GUARD with cleared outputs */
    matched = 77u;
    cjang_bridge_err_t rc3 = cjang_bridge_inspect_and_report(
        "{\"mode\":\"fast\"}",
        "target",
        k_bridge_stops,
        sizeof(k_bridge_stops) / sizeof(k_bridge_stops[0]),
        NULL,
        diag,
        sizeof(diag),
        report,
        sizeof(report),
        &matched
    );
    assert(rc3 == CJANG_BRIDGE_ERR_GUARD);
    assert(matched == 0u);
    assert(report[0] == '\0');

    /* 4. Invalid args reset outputs cleanly */
    matched = 44u;
    cjang_bridge_err_t rc4 = cjang_bridge_inspect_and_report(
        "{\"target\":\"guard_\"}",
        "target",
        NULL,
        0u,
        "guard_",
        diag,
        sizeof(diag),
        report,
        sizeof(report),
        &matched
    );
    assert(rc4 == CJANG_BRIDGE_ERR_ARGS);
    assert(matched == 0u);
    assert(diag[0] == '\0');
    assert(report[0] == '\0');

    printf("[OK] cjang_bridge integration tests passed cleanly.\n");
    return 0;
}
