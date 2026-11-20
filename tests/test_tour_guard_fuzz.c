#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef FUZZ_ITERS
#define FUZZ_ITERS 150000
#endif

static uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void build_mutated_payload(uint32_t *rng, char *buf, size_t cap) {
    static const char alphabet[] =
        "{\"cmd\":\"ping\",\"mode\":\"fast\",\"route\":\"guard_x\",\"tail\":\"abc123\"} "
        "key=val,mode:turbo;err=none\n\t[]{}";
    size_t alpha_len = sizeof(alphabet) - 1u;
    size_t len = (size_t)(xorshift32(rng) % (uint32_t)(cap - 1u)) + 1u;

    for (size_t i = 0; i < len; i++) {
        uint32_t r = xorshift32(rng);
        if ((r & 31u) == 0u) {
            buf[i] = (char)(1u + (r % 126u));
        } else {
            buf[i] = alphabet[r % (uint32_t)alpha_len];
        }
    }
    if ((xorshift32(rng) & 7u) == 0u) {
        buf[len / 2u] = '\0';
    }
    buf[len] = '\0';
}

int main(void) {
    uint32_t rng = 0x5f3759dfu;

    const char *keys[] = {"mode", "cmd", "route", "missing", "", NULL};
    const char *prefixes[] = {"guard_", "tour_", "api_", "none_", "", NULL};

    const cjang_tour_stop_t stops[] = {
        {"guard_kv",     "Guard KV Extractor",   "src/cjang_guard.c", "Extract KV"},
        {"guard_trace",  "Guard Trace Pipeline", "src/cjang_guard.c", "Trace KV"},
        {"tour_prefix",  "Tour Prefix Filter",   "src/cjang_tour.c",  "Filter stops"},
        {"tour_report",  "Tour Summary Report",  "src/cjang_tour.c",  "Render report"},
        {"api_worker",   "API Worker Loop",      "src/api_agent.c",   "Worker loop"},
        {NULL,           "Null Key Entry",       "src/cjang_tour.c",  "Safe skip"}
    };
    const size_t stop_count = sizeof(stops) / sizeof(stops[0]);

    uint64_t guard_ok = 0, guard_err = 0;
    uint64_t tour_ok = 0, tour_err = 0;

    for (uint64_t iter = 0; iter < (uint64_t)FUZZ_ITERS; iter++) {
        char raw[160];
        build_mutated_payload(&rng, raw, sizeof(raw));

        const char *key = keys[xorshift32(&rng) % 6u];

        char val_buf[64];
        size_t val_cap = (size_t)(xorshift32(&rng) % (sizeof(val_buf) + 1u));
        memset(val_buf, 'Z', sizeof(val_buf));
        if (val_cap > 0u) {
            val_buf[val_cap - 1u] = '\0';
        }

        char diag_buf[256];
        size_t diag_cap = (size_t)(xorshift32(&rng) % 128u) + 1u;
        memset(diag_buf, 'X', sizeof(diag_buf));
        diag_buf[diag_cap - 1u] = '\0';

        cjang_guard_err_t grc = cjang_guard_trace_kv(
            raw,
            key,
            (val_cap > 0u) ? val_buf : NULL,
            val_cap,
            diag_buf,
            diag_cap
        );

        assert(diag_buf[diag_cap - 1u] == '\0' || strlen(diag_buf) < diag_cap);
        if (grc == CJANG_GUARD_OK) {
            guard_ok++;
            assert(val_cap > 0u);
            assert(diag_buf[0] != '\0');
            assert(strstr(diag_buf, "key=") != NULL);
            assert(strstr(diag_buf, "vlen=") != NULL);
        } else {
            guard_err++;
            if (diag_buf[0] != '\0') {
                assert(strlen(diag_buf) < diag_cap);
                assert(strstr(diag_buf, "vlen=0") != NULL);
            }
        }

        const char *pfx = prefixes[xorshift32(&rng) % 6u];
        char rep_buf[512];
        size_t rep_cap = (size_t)(xorshift32(&rng) % 240u) + 1u;
        memset(rep_buf, 'Y', sizeof(rep_buf));
        rep_buf[rep_cap - 1u] = '\0';

        size_t matched = 123u;
        int trc = cjang_tour_render_report(
            stops,
            stop_count,
            pfx,
            rep_buf,
            rep_cap,
            &matched
        );

        if (trc == CJANG_TOUR_OK) {
            tour_ok++;
            assert(matched >= 1u && matched <= stop_count);
            assert(rep_buf[0] != '\0');
            assert(strlen(rep_buf) < rep_cap);
            assert(strstr(rep_buf, "[summary]") != NULL);
        } else {
            tour_err++;
            assert(matched == 0u);
            assert(rep_buf[0] == '\0');
        }
    }

    printf("[FUZZ] iters=%llu | guard_ok=%llu guard_err=%llu | tour_ok=%llu tour_err=%llu\n",
           (unsigned long long)FUZZ_ITERS,
           (unsigned long long)guard_ok,
           (unsigned long long)guard_err,
           (unsigned long long)tour_ok,
           (unsigned long long)tour_err);
    printf("[OK] Fuzz invariants passed with zero violations.\n");
    return 0;
}
