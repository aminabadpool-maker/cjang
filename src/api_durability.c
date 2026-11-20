#include "api_durability.h"
#include <stdio.h>
#include <string.h>

static void set_reason(cjang_durability_t *st, const char *reason) {
    if (!st) return;
    const char *src = (reason && reason[0] != '\0') ? reason : "unspecified";
    size_t n = strlen(src);
    if (n >= CJANG_DUR_REASON_MAX) {
        n = CJANG_DUR_REASON_MAX - 1;
    }
    memcpy(st->last_reason, src, n);
    st->last_reason[n] = '\0';
}

void cjang_durability_init(cjang_durability_t *st) {
    if (!st) return;
    memset(st, 0, sizeof(*st));
    snprintf(st->last_reason, sizeof(st->last_reason), "clean");
}

int cjang_durability_check_bounds(
    cjang_durability_t *st,
    size_t len,
    size_t cap,
    const char *reason
) {
    if (!st) return -1;
    st->total_checks++;
    if (cap == 0 || len > cap) {
        st->rejected_checks++;
        st->bound_violations++;
        set_reason(st, reason ? reason : "bounds_exceeded");
        return -1;
    }
    st->ok_checks++;
    return 0;
}

int cjang_durability_check_cstr(
    cjang_durability_t *st,
    const char *s,
    size_t max_len,
    const char *reason
) {
    if (!st) return -1;
    st->total_checks++;
    if (!s || max_len == 0) {
        st->rejected_checks++;
        st->bound_violations++;
        set_reason(st, reason ? reason : "null_or_zero_cap");
        return -1;
    }
    size_t len = 0;
    while (len < max_len && s[len] != '\0') {
        len++;
    }
    if (s[len] != '\0') {
        st->rejected_checks++;
        st->bound_violations++;
        set_reason(st, reason ? reason : "unterminated_or_too_long");
        return -1;
    }
    st->ok_checks++;
    return 0;
}

size_t cjang_durability_summary(
    const cjang_durability_t *st,
    char *out,
    size_t out_cap
) {
    if (!st || !out || out_cap == 0) return 0;
    int written = snprintf(
        out,
        out_cap,
        "total=%llu ok=%llu rej=%llu bounds=%llu last=%s",
        (unsigned long long)st->total_checks,
        (unsigned long long)st->ok_checks,
        (unsigned long long)st->rejected_checks,
        (unsigned long long)st->bound_violations,
        st->last_reason
    );
    if (written < 0) {
        out[0] = '\0';
        return 0;
    }
    if ((size_t)written >= out_cap) {
        return out_cap - 1;
    }
    return (size_t)written;
}
