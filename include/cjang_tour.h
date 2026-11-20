#ifndef CJANG_TOUR_H
#define CJANG_TOUR_H

#include <stddef.h>

typedef struct {
    const char *key;
    const char *title;
    const char *target;
    const char *tip;
} cjang_tour_stop_t;

enum {
    CJANG_TOUR_OK = 0,
    CJANG_TOUR_ERR_ARG = -1,
    CJANG_TOUR_ERR_NOSPACE = -2
};

int cjang_tour_find_stop(const cjang_tour_stop_t *stops, size_t count, const char *key);

int cjang_tour_render_stop(
    const cjang_tour_stop_t *stop,
    size_t index,
    size_t total,
    char *out,
    size_t cap
);

int cjang_tour_render_all(
    const cjang_tour_stop_t *stops,
    size_t count,
    char *out,
    size_t cap
);

int cjang_tour_format_index(
    const cjang_tour_stop_t *stops,
    size_t count,
    char *out,
    size_t cap
);

int cjang_tour_render_prefix(
    const cjang_tour_stop_t *stops,
    size_t count,
    const char *prefix,
    char *out,
    size_t cap
);

#endif /* CJANG_TOUR_H */
