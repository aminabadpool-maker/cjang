#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const cjang_tour_stop_t k_edge_stops[] = {
    {"guard_kv",     "Guard KV Extractor",   "src/cjang_guard.c", "Extract key/value safely"},
    {"guard_trace",  "Guard Trace Pipeline", "src/cjang_guard.c", "Emit key/status/reason trace"},
    {"tour_prefix",  "Tour Prefix Filter",   "src/cjang_tour.c",  "Filter stops by key prefix"},
    {"tour_report",  "Tour Summary Report",  "src/cjang_tour.c",  "Render matched stops + summary"},
    {NULL,           "Null Key Stop",        "src/cjang_tour.c",  "Should safely skip null key"}
};

static void test_guard_trace_edges(void) {
    char val[32];
    char diag[192];
    char tiny_diag[8];

    /* Empty / null raw payload */
    cjang_guard_err_t rc_empty = cjang_guard_trace_kv("", "mode", val, sizeof(val), diag, sizeof(diag));
    assert(rc_empty != CJANG_GUARD_OK);
    assert(diag[0] != '\0');
    assert(strstr(diag, "key=mode") != NULL);
    assert(strstr(diag, "vlen=0") != NULL);

    /* Missing key name */
    cjang_guard_err_t rc_nokey = cjang_guard_trace_kv("{\"mode\":\"fast\"}", "", val, sizeof(val), diag, sizeof(diag));
    assert(rc_nokey != CJANG_GUARD_OK);
    assert(strstr(diag, "<missing_key>") != NULL);
    assert(strstr(diag, "klen=0") != NULL);

    /* Tiny diagnostic buffer must clear output and report too-long */
    cjang_guard_err_t rc_tiny = cjang_guard_trace_kv(
        "{\"mode\":\"fast\"}",
        "mode",
        val,
        sizeof(val),
        tiny_diag,
        sizeof(tiny_diag)
    );
    assert(rc_tiny == CJANG_GUARD_ERR_TOO_LONG);
    assert(tiny_diag[0] == '\0');
}

static void test_tour_report_edges(void) {
    char rep[512];
    char tight_rep[36];
    size_t matched = 77u;

    /* Empty prefix should fail and reset matched count */
    int rc_empty_pfx = cjang_tour_render_report(
        k_edge_stops,
        sizeof(k_edge_stops) / sizeof(k_edge_stops[0]),
        "",
        rep,
        sizeof(rep),
        &matched
    );
    assert(rc_empty_pfx != CJANG_TOUR_OK);
    assert(matched == 0u);
    assert(rep[0] == '\0');

    /* Tight buffer should fail with NOSPACE, clear out, and reset matched */
    matched = 55u;
    int rc_tight = cjang_tour_render_report(
        k_edge_stops,
        sizeof(k_edge_stops) / sizeof(k_edge_stops[0]),
        "guard_",
        tight_rep,
        sizeof(tight_rep),
        &matched
    );
    assert(rc_tight == CJANG_TOUR_ERR_NOSPACE);
    assert(matched == 0u);
    assert(tight_rep[0] == '\0');

    /* Valid prefix skips NULL key entry cleanly */
    matched = 0u;
    int rc_ok = cjang_tour_render_report(
        k_edge_stops,
        sizeof(k_edge_stops) / sizeof(k_edge_stops[0]),
        "tour_",
        rep,
        sizeof(rep),
        &matched
    );
    assert(rc_ok == CJANG_TOUR_OK);
    assert(matched == 2u);
    assert(strstr(rep, "tour_prefix") != NULL);
    assert(strstr(rep, "tour_report") != NULL);
}

static void test_combined_agent_flow(void) {
    char val[32];
    char diag[192];
    char rep[512];
    size_t matched = 0u;

    cjang_guard_err_t grc = cjang_guard_trace_kv(
        "{\"route\":\"tour_\",\"mode\":\"strict\"}",
        "route",
        val,
        sizeof(val),
        diag,
        sizeof(diag)
    );
    assert(grc == CJANG_GUARD_OK);
    assert(strcmp(val, "tour_") == 0);

    int trc = cjang_tour_render_report(
        k_edge_stops,
        sizeof(k_edge_stops) / sizeof(k_edge_stops[0]),
        val,
        rep,
        sizeof(rep),
        &matched
    );
    assert(trc == CJANG_TOUR_OK);
    assert(matched == 2u);
    assert(strstr(rep, "[summary] prefix=tour_ matched=2") != NULL);
}

int main(void) {
    test_guard_trace_edges();
    test_tour_report_edges();
    test_combined_agent_flow();
    printf("[OK] CLI and pipeline edge tests passed cleanly.\n");
    return 0;
}
