#ifndef CJANG_GUARD_H
#define CJANG_GUARD_H

#include <stddef.h>

enum {
    CJANG_GUARD_MAX_BUF = 65536,
    CJANG_GUARD_MAX_KEY = 128,
    CJANG_GUARD_MAX_VAL = 4096
};

typedef enum {
    CJANG_GUARD_OK = 0,
    CJANG_GUARD_ERR_NULL = -1,
    CJANG_GUARD_ERR_BOUNDS = -2,
    CJANG_GUARD_ERR_TOO_LONG = -3,
    CJANG_GUARD_ERR_SYNTAX = -4,
    CJANG_GUARD_ERR_NOT_FOUND = -5
} cjang_guard_err_t;

const char *cjang_guard_strerror(cjang_guard_err_t err);

cjang_guard_err_t cjang_guard_validate_buf(
    const char *buf,
    size_t len,
    size_t max_len
);

cjang_guard_err_t cjang_guard_extract_kv(
    const char *buf,
    size_t len,
    const char *key,
    char *out,
    size_t out_cap
);

cjang_guard_err_t cjang_guard_format_diag(
    const char *key,
    cjang_guard_err_t err,
    char *out,
    size_t cap
);

#endif /* CJANG_GUARD_H */
