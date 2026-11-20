#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef WORKERS
#define WORKERS 4
#endif

#ifndef ITERS_PER_WORKER
#define ITERS_PER_WORKER 100000
#endif

typedef struct {
    int id;
    uint64_t trace_ok;
    uint64_t trace_err;
    uint64_t report_ok;
    uint64_t report_err;
} worker_ctx_t;

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

static void *worker_fn(void *arg) {
    worker_ctx_t *ctx = (worker_ctx_t *)arg;

    const cjang_tour_stop_t stops[] = {
        {"guard_kv", "Guard KV Extractor", "src/cjang_guard.c", "Safe KV parse"},
        {"guard_diag", "Guard Diagnostics", "src/cjang_guard.c", "Error trace"},
        {"tour_index", "Tour Indexer", "src/cjang_tour.c", "Stop listing"},
        {"tour_report", "Tour Summary Report", "src/cjang_tour.c", "Prefix summary"}
    };

    const char *raw_samples[] = {
        "{\"role\":\"agent\",\"mode\":\"fast\"}",
        "{\"role\":\"agent\"}",
        "bad_payload_without_kv",
        "{\"role\":\"agent\",\"mode\":\"turbo_mode_value\"}"
    };

    const char *prefixes[] = {"guard_", "tour_", "none_"};
    uint32_t rng = (uint32_t)(0x9e3779b9u ^ (uint32_t)(ctx->id + 1) * 2654435761u);

    char val_buf[32];
    char diag_buf[192];
    char tiny_diag[12];
    char rep_buf[512];
    char tiny_rep[24];

    for (uint64_t i = 0; i < (uint64_t)ITERS_PER_WORKER; i++) {
        rng ^= rng << 13;
        rng ^= rng >> 17;
        rng ^= rng << 5;

        const char *raw = raw_samples[rng % 4u];
        int use_tiny_diag = ((rng >> 4) & 7u) == 0u;

        cjang_guard_err_t grc = use_tiny_diag
            ? cjang_guard_trace_kv(raw, "mode", val_buf, sizeof(val_buf), tiny_diag, sizeof(tiny_diag))
            : cjang_guard_trace_kv(raw, "mode", val_buf, sizeof(val_buf), diag_buf, sizeof(diag_buf));

        if (grc == CJANG_GUARD_OK) {
            ctx->trace_ok++;
            assert(diag_buf[0] != '\0');
            assert(strstr(diag_buf, "key=mode") != NULL);
        } else {
            ctx->trace_err++;
            if (use_tiny_diag) {
                assert(tiny_diag[0] == '\0');
            } else {
                assert(diag_buf[0] != '\0');
            }
        }

        const char *pfx = prefixes[(rng >> 8) % 3u];
        int use_tiny_rep = ((rng >> 12) & 7u) == 0u;
        size_t matched = 0;

        int trc = use_tiny_rep
            ? cjang_tour_render_report(stops, 4, pfx, tiny_rep, sizeof(tiny_rep), &matched)
            : cjang_tour_render_report(stops, 4, pfx, rep_buf, sizeof(rep_buf), &matched);

        if (trc == CJANG_TOUR_OK) {
            ctx->report_ok++;
            assert(matched == 2u);
            assert(strstr(rep_buf, "[summary]") != NULL);
        } else {
            ctx->report_err++;
            if (use_tiny_rep) {
                assert(tiny_rep[0] == '\0');
            }
            assert(matched == 0u);
        }
    }

    return NULL;
}

int main(void) {
    pthread_t th[WORKERS];
    worker_ctx_t wctx[WORKERS];
    memset(wctx, 0, sizeof(wctx));

    double t0 = now_ms();
    for (int i = 0; i < WORKERS; i++) {
        wctx[i].id = i;
        assert(pthread_create(&th[i], NULL, worker_fn, &wctx[i]) == 0);
    }
    for (int i = 0; i < WORKERS; i++) {
        assert(pthread_join(th[i], NULL) == 0);
    }
    double elapsed_ms = now_ms() - t0;

    uint64_t total_trace_ok = 0, total_trace_err = 0;
    uint64_t total_rep_ok = 0, total_rep_err = 0;
    for (int i = 0; i < WORKERS; i++) {
        total_trace_ok += wctx[i].trace_ok;
        total_trace_err += wctx[i].trace_err;
        total_rep_ok += wctx[i].report_ok;
        total_rep_err += wctx[i].report_err;
    }

    uint64_t total_cycles = (uint64_t)WORKERS * (uint64_t)ITERS_PER_WORKER;
    double ops_per_sec = elapsed_ms > 0.0 ? ((double)total_cycles * 1000.0) / elapsed_ms : 0.0;

    assert(total_trace_ok + total_trace_err == total_cycles);
    assert(total_rep_ok + total_rep_err == total_cycles);

    printf("[MT-PIPE] workers=%d | cycles=%llu | time=%.2f ms | cycles/s=%.0f\n",
           WORKERS,
           (unsigned long long)total_cycles,
           elapsed_ms,
           ops_per_sec);
    printf("[TRACE]  ok=%llu | err_or_bounded=%llu\n",
           (unsigned long long)total_trace_ok,
           (unsigned long long)total_trace_err);
    printf("[REPORT] ok=%llu | err_or_bounded=%llu\n",
           (unsigned long long)total_rep_ok,
           (unsigned long long)total_rep_err);
    printf("[OK] Multi-threaded pipeline stress test passed with zero errors.\n");
    return 0;
}
