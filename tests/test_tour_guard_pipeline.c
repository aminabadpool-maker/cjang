#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char val[64];
    char diag[256];

    cjang_guard_err_t rc = cjang_guard_trace_kv(
        "{\"role\":\"agent\",\"mode\":\"fast\"}",
        "mode",
        val,
        sizeof(val),
        diag,
        sizeof(diag)
    );
    assert(rc == CJANG_GUARD_OK);
    assert(strcmp(val, "fast") == 0);
    assert(strstr(diag, "key=mode") != NULL);
    assert(strstr(diag, "vlen=4") != NULL);

    cjang_guard_err_t miss_rc = cjang_guard_trace_kv(
        "{\"role\":\"agent\"}",
        "mode",
        val,
        sizeof(val),
        diag,
        sizeof(diag)
    );
    assert(miss_rc != CJANG_GUARD_OK);
    assert(strstr(diag, "key=mode") != NULL);
    assert(strstr(diag, "vlen=0") != NULL);

    const cjang_tour_stop_t stops[] = {
        {"guard_kv", "Guard KV Extractor", "src/cjang_guard.c", "Safe KV parse"},
        {"guard_diag", "Guard Diagnostics", "src/cjang_guard.c", "Error trace"},
        {"tour_index", "Tour Indexer", "src/cjang_tour.c", "Stop listing"}
    };

    char report[512];
    size_t matched = 0;
    assert(cjang_tour_render_report(stops, 3, "guard_", report, sizeof(report), &matched) == CJANG_TOUR_OK);
    assert(matched == 2);
    assert(strstr(report, "guard_kv") != NULL);
    assert(strstr(report, "guard_diag") != NULL);
    assert(strstr(report, "[summary] prefix=guard_ matched=2") != NULL);
    assert(strstr(report, "tour_index") == NULL);

    printf("[OK] tour/guard pipeline passed all checks.\n");
    return 0;
}
