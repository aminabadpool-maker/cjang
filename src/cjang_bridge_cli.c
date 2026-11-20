#include "cjang_bridge.h"

#include <stdio.h>
#include <string.h>

#define CJANG_MAX_CANDIDATES 16u
#define CJANG_STDIN_BUF_CAP 2048u

static const cjang_tour_stop_t k_default_stops[] = {
    {"guard_kv",     "Guard KV Extractor",   "src/cjang_guard.c",  "Extract key/value safely"},
    {"guard_trace",  "Guard Trace Pipeline", "src/cjang_guard.c",  "Emit key/status/reason trace"},
    {"tour_prefix",  "Tour Prefix Filter",   "src/cjang_tour.c",   "Filter stops by key prefix"},
    {"tour_report",  "Tour Summary Report",  "src/cjang_tour.c",   "Render matched stops + summary"},
    {"bridge_route", "Bridge Route Helper",  "src/cjang_bridge.c", "Combine guard trace & tour report"}
};

static void print_usage(FILE *out) {
    fprintf(
        out,
        "usage: cjang_bridge_cli --raw <payload|-> --key <name> [--prefix <cand>]... [-q|--quiet]\n"
    );
}

static int read_stdin_bounded(char *out, size_t cap) {
    if (out == NULL || cap == 0u) {
        return -1;
    }
    out[0] = '\0';

    size_t used = 0u;
    while (used + 1u < cap) {
        size_t n = fread(out + used, 1u, (cap - 1u) - used, stdin);
        if (n == 0u) {
            break;
        }
        used += n;
    }
    out[used] = '\0';

    if (ferror(stdin)) {
        out[0] = '\0';
        return -1;
    }
    return 0;
}

int cjang_bridge_cli_main(int argc, char **argv) {
    const char *raw = NULL;
    const char *key = NULL;
    const char *candidates[CJANG_MAX_CANDIDATES];
    size_t cand_count = 0u;
    int quiet = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--raw") == 0) {
            if (i + 1 >= argc) {
                print_usage(stderr);
                return 2;
            }
            raw = argv[++i];
        } else if (strcmp(argv[i], "--key") == 0) {
            if (i + 1 >= argc) {
                print_usage(stderr);
                return 2;
            }
            key = argv[++i];
        } else if (strcmp(argv[i], "--prefix") == 0) {
            if (i + 1 >= argc) {
                print_usage(stderr);
                return 2;
            }
            if (cand_count >= CJANG_MAX_CANDIDATES) {
                fprintf(stderr, "[ERR] too many --prefix arguments (max %u)\n",
                        (unsigned)CJANG_MAX_CANDIDATES);
                return 2;
            }
            candidates[cand_count++] = argv[++i];
        } else if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
            quiet = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(stdout);
            return 0;
        } else {
            fprintf(stderr, "[ERR] unknown argument: %s\n", argv[i]);
            print_usage(stderr);
            return 2;
        }
    }

    if (raw == NULL || key == NULL || key[0] == '\0') {
        print_usage(stderr);
        return 2;
    }

    char stdin_buf[CJANG_STDIN_BUF_CAP];
    stdin_buf[0] = '\0';

    if (strcmp(raw, "-") == 0) {
        if (read_stdin_bounded(stdin_buf, sizeof(stdin_buf)) != 0) {
            fprintf(stderr, "[ERR] failed reading payload from stdin\n");
            return 2;
        }
        raw = stdin_buf;
    }

    char diag[256];
    char report[1024];
    size_t matched = 0u;

    cjang_bridge_err_t rc = cjang_bridge_inspect_multi_prefix(
        raw,
        key,
        k_default_stops,
        sizeof(k_default_stops) / sizeof(k_default_stops[0]),
        candidates,
        cand_count,
        diag,
        sizeof(diag),
        report,
        sizeof(report),
        &matched
    );

    if (!quiet && diag[0] != '\0') {
        printf("[DIAG] %s\n", diag);
    }

    if (rc == CJANG_BRIDGE_OK) {
        printf("[MATCHED] %zu\n%s", matched, report);
        return 0;
    }

    if (rc == CJANG_BRIDGE_ERR_ARGS) {
        fprintf(stderr, "[ERR] bridge invalid arguments\n");
        return 2;
    }

    fprintf(stderr, "[ERR] bridge inspection failed (code=%d, matched=%zu)\n",
            (int)rc, matched);
    return 3;
}

#ifndef CJANG_BRIDGE_CLI_NO_MAIN
int main(int argc, char **argv) {
    return cjang_bridge_cli_main(argc, argv);
}
#endif
