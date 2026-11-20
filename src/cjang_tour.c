#include "cjang_tour.h"

#include <stdio.h>
#include <string.h>

int cjang_tour_find_stop(const cjang_tour_stop_t *stops, size_t count, const char *key) {
    if (!stops || count == 0 || !key || key[0] == '\0') {
        return CJANG_TOUR_ERR_ARG;
    }

    for (size_t i = 0; i < count; i++) {
        if (stops[i].key != NULL && strcmp(stops[i].key, key) == 0) {
            return (int)i;
        }
    }

    return CJANG_TOUR_ERR_ARG;
}

int cjang_tour_render_stop(
    const cjang_tour_stop_t *stop,
    size_t index,
    size_t total,
    char *out,
    size_t cap
) {
    if (!stop || !stop->title || !stop->target || !stop->tip || !out || cap == 0) {
        return CJANG_TOUR_ERR_ARG;
    }
    if (total == 0 || index >= total) {
        return CJANG_TOUR_ERR_ARG;
    }

    out[0] = '\0';

    int needed = snprintf(
        out,
        cap,
        "[%zu/%zu] %s\n  target: %s\n  note  : %s\n",
        index + 1,
        total,
        stop->title,
        stop->target,
        stop->tip
    );

    if (needed < 0) {
        out[0] = '\0';
        return CJANG_TOUR_ERR_ARG;
    }
    if ((size_t)needed >= cap) {
        out[0] = '\0';
        return CJANG_TOUR_ERR_NOSPACE;
    }

    return CJANG_TOUR_OK;
}

int cjang_tour_render_all(
    const cjang_tour_stop_t *stops,
    size_t count,
    char *out,
    size_t cap
) {
    if (!stops || count == 0 || !out || cap == 0) {
        return CJANG_TOUR_ERR_ARG;
    }

    out[0] = '\0';
    size_t used = 0;

    for (size_t i = 0; i < count; i++) {
        char item_buf[512];
        int rc = cjang_tour_render_stop(&stops[i], i, count, item_buf, sizeof(item_buf));
        if (rc != CJANG_TOUR_OK) {
            out[0] = '\0';
            return rc;
        }

        const char *sep = (i + 1 < count) ? "----------------------------------------\n" : "";
        size_t item_len = strlen(item_buf);
        size_t sep_len = strlen(sep);

        if (used + item_len + sep_len + 1 > cap) {
            out[0] = '\0';
            return CJANG_TOUR_ERR_NOSPACE;
        }

        memcpy(out + used, item_buf, item_len);
        used += item_len;
        if (sep_len > 0) {
            memcpy(out + used, sep, sep_len);
            used += sep_len;
        }
        out[used] = '\0';
    }

    return CJANG_TOUR_OK;
}

int cjang_tour_format_index(
    const cjang_tour_stop_t *stops,
    size_t count,
    char *out,
    size_t cap
) {
    if (!stops || count == 0 || !out || cap == 0) {
        if (out && cap > 0) {
            out[0] = '\0';
        }
        return CJANG_TOUR_ERR_ARG;
    }

    out[0] = '\0';
    size_t used = 0;

    for (size_t i = 0; i < count; i++) {
        const char *k = stops[i].key ? stops[i].key : "(null)";
        const char *t = stops[i].title ? stops[i].title : "(untitled)";
        char line[256];

        int n = snprintf(line, sizeof(line), "  - %-10s : %s\n", k, t);
        if (n < 0) {
            out[0] = '\0';
            return CJANG_TOUR_ERR_ARG;
        }
        if ((size_t)n >= sizeof(line) || used + (size_t)n + 1 > cap) {
            out[0] = '\0';
            return CJANG_TOUR_ERR_NOSPACE;
        }

        size_t llen = (size_t)n;
        memcpy(out + used, line, llen);
        used += llen;
        out[used] = '\0';
    }

    return CJANG_TOUR_OK;
}

int cjang_tour_render_prefix(
    const cjang_tour_stop_t *stops,
    size_t count,
    const char *prefix,
    char *out,
    size_t cap
) {
    if (!stops || count == 0 || !prefix || prefix[0] == '\0' || !out || cap == 0) {
        if (out && cap > 0) {
            out[0] = '\0';
        }
        return CJANG_TOUR_ERR_ARG;
    }

    out[0] = '\0';
    size_t used = 0;
    size_t plen = strlen(prefix);
    int matched = 0;

    for (size_t i = 0; i < count; i++) {
        const char *k = stops[i].key;
        if (!k || strncmp(k, prefix, plen) != 0) {
            continue;
        }
        matched = 1;
        const char *t = stops[i].title ? stops[i].title : "(untitled)";
        char line[256];
        int n = snprintf(line, sizeof(line), "  * %s -> %s\n", k, t);
        if (n < 0) {
            out[0] = '\0';
            return CJANG_TOUR_ERR_ARG;
        }
        if ((size_t)n >= sizeof(line) || used + (size_t)n + 1 > cap) {
            out[0] = '\0';
            return CJANG_TOUR_ERR_NOSPACE;
        }
        size_t llen = (size_t)n;
        memcpy(out + used, line, llen);
        used += llen;
        out[used] = '\0';
    }

    if (!matched) {
        out[0] = '\0';
        return CJANG_TOUR_ERR_ARG;
    }

    return CJANG_TOUR_OK;
}
