#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const cjang_tour_stop_t k_stops[] = {
    {"guard_kv",     "Guard KV Extractor",   "src/cjang_guard.c", "Extract key/value safely"},
    {"guard_trace",  "Guard Trace Pipeline", "src/cjang_guard.c", "Emit key/status/reason trace"},
    {"tour_prefix",  "Tour Prefix Filter",   "src/cjang_tour.c",  "Filter stops by key prefix"},
    {"tour_report",  "Tour Summary Report",  "src/cjang_tour.c",  "Render matched stops + summary"},
    {"agent_router", "Agent Dispatch Loop",  "src/api_agent.c",   "Route agent commands"}
};

static int run_once(const char *raw, const char *key, const char *prefix) {
    char val_buf[64];
    char diag_buf[256];
    char rep_buf[512];
    size_t matched = 0;

    cjang_guard_err_t grc = cjang_guard_trace_kv(
        raw,
        key,
        val_buf,
        sizeof(val_buf),
        diag_buf,
        sizeof(diag_buf)
    );

    printf("--- GUARD TRACE ---\n");
    if (diag_buf[0] != '\0') {
        fputs(diag_buf, stdout);
    }
    if (grc == CJANG_GUARD_OK) {
        printf("extracted_val=%s\n", val_buf);
    }

    int trc = cjang_tour_render_report(
        k_stops,
        sizeof(k_stops) / sizeof(k_stops[0]),
        prefix,
        rep_buf,
        sizeof(rep_buf),
        &matched
    );

    printf("--- TOUR REPORT ---\n");
    if (trc == CJANG_TOUR_OK && rep_buf[0] != '\0') {
        fputs(rep_buf, stdout);
    } else {
        printf("[summary] prefix=%s matched=%zu status=%d\n", prefix ? prefix : "(null)", matched, trc);
    }

    return (grc == CJANG_GUARD_OK && trc == CJANG_TOUR_OK) ? 0 : 1;
}

static void run_self_checks(void) {
    char val_buf[32];
    char diag_buf[192];
    char rep_buf[512];
    size_t matched = 0;

    /* 1. Happy path: valid KV + matching tour prefix */
    cjang_guard_err_t grc = cjang_guard_trace_kv(
        "{\"cmd\":\"ping\",\"mode\":\"fast\"}",
        "mode",
        val_buf,
        sizeof(val_buf),
        diag_buf,
        sizeof(diag_buf)
    );
    assert(grc == CJANG_GUARD_OK);
    assert(strcmp(val_buf, "fast") == 0);
    assert(strstr(diag_buf, "status=0") != NULL);

    int trc = cjang_tour_render_report(
        k_stops,
        sizeof(k_stops) / sizeof(k_stops[0]),
        "guard_",
        rep_buf,
        sizeof(rep_buf),
        &matched
    );
    assert(trc == CJANG_TOUR_OK);
    assert(matched == 2u);
    assert(strstr(rep_buf, "guard_kv") != NULL);
    assert(strstr(rep_buf, "guard_trace") != NULL);

    /* 2. Missing key should produce non-zero status and vlen=0 without UB */
    char miss_val[16];
    char miss_diag[192];
    cjang_guard_err_t mrc = cjang_guard_trace_kv(
        "{\"cmd\":\"ping\"}",
        "mode",
        miss_val,
        sizeof(miss_val),
        miss_diag,
        sizeof(miss_diag)
    );
    assert(mrc != CJANG_GUARD_OK);
    assert(strstr(miss_diag, "vlen=0") != NULL);

    /* 3. Non-matching prefix resets matched count to 0 */
    char empty_rep[128];
    size_t miss_matched = 99u;
    int erc = cjang_tour_render_report(
        k_stops,
        sizeof(k_stops) / sizeof(k_stops[0]),
        "unknown_",
        empty_rep,
        sizeof(empty_rep),
        &miss_matched
    );
    assert(erc != CJANG_TOUR_OK);
    assert(miss_matched == 0u);
    assert(empty_rep[0] == '\0');

    printf("[OK] CLI diagnostic runner self-checks passed.\n");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        run_self_checks();
        return 0;
    }

    const char *raw = "{\"role\":\"agent\",\"mode\":\"fast\"}";
    const char *key = "mode";
    const char *prefix = "guard_";

    for (int i = 1; i + 1 < argc; i += 2) {
        if (strcmp(argv[i], "--raw") == 0) {
            raw = argv[i + 1];
        } else if (strcmp(argv[i], "--key") == 0) {
            key = argv[i + 1];
        } else if (strcmp(argv[i], "--prefix") == 0) {
            prefix = argv[i + 1];
        }
    }

    return run_once(raw, key, prefix);
}
