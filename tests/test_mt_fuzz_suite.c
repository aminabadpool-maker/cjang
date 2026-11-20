#define _POSIX_C_SOURCE 200809L

#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum {
    WORKER_COUNT = 4,
    ITERS_PER_WORKER = 150000
};

typedef struct {
    int tid;
    size_t iters;
    uint32_t seed;
    size_t tour_ok;
    size_t tour_err;
    size_t guard_ok;
    size_t guard_err;
} worker_ctx_t;

static const cjang_tour_stop_t g_stops[] = {
    {
        "routes",
        "Request Routing & Handlers",
        "src/api_tcp.c & src/api_mw.c",
        "Explicit route dispatch and middleware flow."
    },
    {
        "agent",
        "Agent Execution & KV Parser",
        "src/api_agent.c (/__agent/exec)",
        "Quoted or unquoted keys with whitespace around ':' or '='."
    },
    {
        "metrics",
        "Runtime Metrics & Health",
        "src/api_metrics.c",
        "Low-overhead runtime counters and health checks."
    },
    {
        "e2e",
        "Loopback & Full Test Suite",
        "tools/run_api_agent.sh & tools/run_e2e_127.sh",
        "End-to-end verification before pushing changes."
    }
};

static const size_t g_stop_count = sizeof(g_stops) / sizeof(g_stops[0]);

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

static uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void exercise_tour(worker_ctx_t *ctx) {
    const char *keys[] = {"routes", "agent", "metrics", "e2e", "missing", ""};
    size_t kcount = sizeof(keys) / sizeof(keys[0]);

    const char *k = keys[xorshift32(&ctx->seed) % kcount];
    int idx = cjang_tour_find_stop(g_stops, g_stop_count, k);

    size_t cap = (xorshift32(&ctx->seed) % 2048) + 1;
    char *out = (char *)malloc(cap);
    assert(out != NULL);

    if ((xorshift32(&ctx->seed) & 1U) == 0) {
        size_t target_idx = (idx >= 0) ? (size_t)idx : (xorshift32(&ctx->seed) % g_stop_count);
        int rc = cjang_tour_render_stop(&g_stops[target_idx], target_idx, g_stop_count, out, cap);
        if (rc == CJANG_TOUR_OK) {
            assert(strlen(out) < cap);
            ctx->tour_ok++;
        } else {
            assert(rc == CJANG_TOUR_ERR_NOSPACE || rc == CJANG_TOUR_ERR_ARG);
            assert(out[0] == '\0');
            ctx->tour_err++;
        }
    } else {
        int rc = cjang_tour_render_all(g_stops, g_stop_count, out, cap);
        if (rc == CJANG_TOUR_OK) {
            assert(strlen(out) < cap);
            ctx->tour_ok++;
        } else {
            assert(rc == CJANG_TOUR_ERR_NOSPACE || rc == CJANG_TOUR_ERR_ARG);
            assert(out[0] == '\0');
            ctx->tour_err++;
        }
    }

    free(out);
}

static void exercise_guard(worker_ctx_t *ctx) {
    const char *seeds[] = {
        "{\"cmd\":\"sync\",\"mode\":\"fast\",\"retry\":2}",
        "{ \"cmd\" : \"flush_metrics\" , target = agent }",
        "{\"nested\":\"{\\\"cmd\\\":\\\"inner\\\"}\",\"cmd\":\"outer\"}",
        "cmd=health_check,timeout=30"
    };
    size_t nseeds = sizeof(seeds) / sizeof(seeds[0]);

    const char *keys[] = {"cmd", "mode", "retry", "target", "timeout", "missing"};
    size_t nkeys = sizeof(keys) / sizeof(keys[0]);

    char payload[512];
    char out[128];

    size_t plen = 0;
    if ((xorshift32(&ctx->seed) & 1U) == 0) {
        const char *s = seeds[xorshift32(&ctx->seed) % nseeds];
        size_t slen = strlen(s);
        assert(slen < sizeof(payload));
        memcpy(payload, s, slen + 1);

        size_t muts = (xorshift32(&ctx->seed) % 3) + 1;
        for (size_t m = 0; m < muts; m++) {
            size_t pos = xorshift32(&ctx->seed) % slen;
            payload[pos] = (char)(32 + (xorshift32(&ctx->seed) % 95));
        }
        plen = (xorshift32(&ctx->seed) % slen) + 1;
        payload[plen] = '\0';
    } else {
        plen = (xorshift32(&ctx->seed) % (sizeof(payload) - 1)) + 1;
        for (size_t i = 0; i < plen; i++) {
            payload[i] = (char)(xorshift32(&ctx->seed) % 256);
        }
        payload[plen] = '\0';
    }

    assert(cjang_guard_validate_buf(payload, plen, CJANG_GUARD_MAX_BUF) == CJANG_GUARD_OK);

    const char *k = keys[xorshift32(&ctx->seed) % nkeys];
    size_t cap = (xorshift32(&ctx->seed) % (sizeof(out) - 1)) + 1;

    cjang_guard_err_t rc = cjang_guard_extract_kv(payload, plen, k, out, cap);
    if (rc == CJANG_GUARD_OK) {
        size_t olen = strlen(out);
        assert(olen > 0 && olen < cap);
        ctx->guard_ok++;
    } else {
        assert(rc == CJANG_GUARD_ERR_NOT_FOUND ||
               rc == CJANG_GUARD_ERR_SYNTAX ||
               rc == CJANG_GUARD_ERR_TOO_LONG ||
               rc == CJANG_GUARD_ERR_BOUNDS ||
               rc == CJANG_GUARD_ERR_NULL);
        assert(out[0] == '\0');
        ctx->guard_err++;
    }
}

static void *worker_entry(void *arg) {
    worker_ctx_t *ctx = (worker_ctx_t *)arg;
    for (size_t i = 0; i < ctx->iters; i++) {
        exercise_tour(ctx);
        exercise_guard(ctx);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[WORKER_COUNT];
    worker_ctx_t ctxs[WORKER_COUNT];

    uint64_t t0 = now_ns();
    for (int t = 0; t < WORKER_COUNT; t++) {
        ctxs[t].tid = t;
        ctxs[t].iters = ITERS_PER_WORKER;
        ctxs[t].seed = 0xA5A50000u ^ (uint32_t)((t + 1) * 0x9E3779B9u);
        ctxs[t].tour_ok = 0;
        ctxs[t].tour_err = 0;
        ctxs[t].guard_ok = 0;
        ctxs[t].guard_err = 0;

        int prc = pthread_create(&threads[t], NULL, worker_entry, &ctxs[t]);
        assert(prc == 0);
    }

    size_t total_tour_ok = 0, total_tour_err = 0;
    size_t total_guard_ok = 0, total_guard_err = 0;

    for (int t = 0; t < WORKER_COUNT; t++) {
        int jrc = pthread_join(threads[t], NULL);
        assert(jrc == 0);
        total_tour_ok += ctxs[t].tour_ok;
        total_tour_err += ctxs[t].tour_err;
        total_guard_ok += ctxs[t].guard_ok;
        total_guard_err += ctxs[t].guard_err;
    }
    uint64_t t1 = now_ns();

    size_t total_pairs = (size_t)WORKER_COUNT * ITERS_PER_WORKER;
    double elapsed_ms = (double)(t1 - t0) / 1000000.0;
    double pair_ops_per_sec = ((double)total_pairs / ((double)(t1 - t0) / 1000000000.0));

    printf("[MT-FUZZ] workers=%d | pairs=%zu | time=%.2f ms | pair_ops/s=%.0f\n",
           WORKER_COUNT, total_pairs, elapsed_ms, pair_ops_per_sec);
    printf("[TOUR]  ok=%zu | rejected_or_nospace=%zu\n", total_tour_ok, total_tour_err);
    printf("[GUARD] ok=%zu | rejected_or_missing=%zu\n", total_guard_ok, total_guard_err);
    printf("[OK] Multi-threaded fuzz suite passed with zero errors.\n");
    return 0;
}
