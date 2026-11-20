#ifndef CJANG_API_DURABILITY_H
#define CJANG_API_DURABILITY_H

#include <stddef.h>
#include <stdint.h>

#define CJANG_DUR_REASON_MAX 64

typedef struct {
    uint64_t total_checks;
    uint64_t ok_checks;
    uint64_t rejected_checks;
    uint64_t bound_violations;
    char last_reason[CJANG_DUR_REASON_MAX];
} cjang_durability_t;

void cjang_durability_init(cjang_durability_t *st);

int cjang_durability_check_bounds(
    cjang_durability_t *st,
    size_t len,
    size_t cap,
    const char *reason
);

int cjang_durability_check_cstr(
    cjang_durability_t *st,
    const char *s,
    size_t max_len,
    const char *reason
);

size_t cjang_durability_summary(
    const cjang_durability_t *st,
    char *out,
    size_t out_cap
);

#endif
