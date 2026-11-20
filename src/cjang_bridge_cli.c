#include "cjang_bridge.h"

#include <stdio.h>
#include <string.h>

#define CJANG_MAX_CANDIDATES 16u
#define CJANG_STDIN_BUF_CAP 2048u

typedef enum {
    CJANG_FMT_RAW = 0,
    CJANG_FMT_COMPACT = 1,
    CJANG_FMT_KV = 2
} cjang_out_fmt_t;

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
        "usage: cjang_bridge_cli --raw <payload|-> --key <name> [--prefix <cand>]... "
        "[-q|--quiet] [-c|--count-only] [-f|--first] [-o|--format raw|compact|kv] [-s|--strict]\n"
    );
}

static int parse_format(const char *s, cjang_out_fmt_t *out_fmt) {
    if (s == NULL || out_fmt == NULL) {
        return -1;
    }
    if (strcmp(s, "raw") == 0) {
        *out_fmt = CJANG_FMT_RAW;
        return 0;
    }
    if (strcmp(s, "compact") == 0) {
        *out_fmt = CJANG_FMT_COMPACT;
        return 0;
    }
    if (strcmp(s, "kv") == 0) {
        *out_fmt = CJANG_FMT_KV;
        return 0;
    }
    return -1;
}

static const char *extract_first_stop_id(const char *report, char *buf, size_t cap) {
    if (report == NULL || buf == NULL || cap == 0u) {
        return "";
    }
    buf[0] = '\0';

    const char *marker = strstr(report, "  * ");
    if (marker == NULL) {
        return "";
    }
    marker += 4;

    const char *end = strstr(marker, " ->");
    size_t len = (end != NULL) ? (size_t)(end - marker) : strlen(marker);
    if (len >= cap) {
        len = cap - 1u;
    }
    memcpy(buf, marker, len);
    buf[len] = '\0';
    return buf;
}

/* Returns 0 on normal read, 1 if buffer reached cap (possible truncation), -1 on error. */
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
    if (used + 1u >= cap) {
        return 1;
    }
    return 0;
}

int cjang_bridge_cli_main(int argc, char **argv) {
    const char *raw = NULL;
    const char *key = NULL;
    const char *candidates[CJANG_MAX_CANDIDATES];
    size_t cand_count = 0u;
    int quiet = 0;
    int count_only = 0;
    int first_only = 0;
    int strict_mode = 0;
    cjang_out_fmt_t fmt = CJANG_FMT_RAW;

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
        } else if (strcmp(argv[i], "--count-only") == 0 || strcmp(argv[i], "-c") == 0) {
            count_only = 1;
        } else if (strcmp(argv[i], "--first") == 0 || strcmp(argv[i], "-f") == 0) {
            first_only = 1;
        } else if (strcmp(argv[i], "--strict") == 0 || strcmp(argv[i], "-s") == 0) {
            strict_mode = 1;
        } else if (strcmp(argv[i], "--format") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc || parse_format(argv[++i], &fmt) != 0) {
                fprintf(stderr, "[ERR] invalid format (expected raw|compact|kv)\n");
                return 2;
            }
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
        int r_status = read_stdin_bounded(stdin_buf, sizeof(stdin_buf));
        if (r_status < 0) {
            fprintf(stderr, "[ERR] failed reading payload from stdin\n");
            return 2;
        }
        if (r_status > 0) {
            if (strict_mode) {
                fprintf(stderr, "[ERR] stdin payload exceeded cap (%u bytes) in strict mode\n",
                        (unsigned)(sizeof(stdin_buf) - 1u));
                return 2;
            }
            if (!quiet) {
                fprintf(stderr, "[WARN] stdin payload truncated at %u bytes\n",
                        (unsigned)(sizeof(stdin_buf) - 1u));
            }
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
        char first_stop[128];
        extract_first_stop_id(report, first_stop, sizeof(first_stop));

        if (count_only) {
            printf("%zu\n", matched);
        } else if (fmt == CJANG_FMT_COMPACT) {
            if (first_only) {
                printf("matched=%zu first=%s\n", matched, first_stop);
            } else {
                printf("matched=%zu first=%s\n%s", matched, first_stop, report);
            }
        } else if (fmt == CJANG_FMT_KV) {
            printf("status=ok key=%s matched=%zu first=%s\n", key, matched, first_stop);
        } else if (first_only) {
            printf("[FIRST] %s\n", first_stop);
        } else {
            printf("[MATCHED] %zu\n%s", matched, report);
        }
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
