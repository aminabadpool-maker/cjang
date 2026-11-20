#ifndef CJANG_API_PLUS_H
#define CJANG_API_PLUS_H

#include "api_ext.h"
#include "api_router.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>

/* 1. Extracts Bearer token from Authorization header. Returns token len or -1. */
int cjang_plus_bearer_get(const char *raw_req, char *out, size_t out_cap);

/* 2. Extracts int64 parameter from URL query/form string. Returns 0 on success, -1 on error. */
int cjang_plus_param_get_i64(const char *kv_str, size_t kv_len, const char *key, int64_t *out_val);

/* 3. Extracts simple flat JSON string value for key. Returns value len or -1. */
int cjang_plus_json_get_str(const char *json, size_t json_len, const char *key, char *out, size_t out_cap);

/* 4. Trims leading and trailing whitespace in-place. Returns new length. */
size_t cjang_plus_trim_inplace(char *s);

/* 5. Safe bounded append into fixed buffer. Returns 0 if fully appended, -1 if truncated. */
int cjang_plus_str_append(char *dst, size_t dst_cap, const char *add);

/* 6. Returns standard HTTP reason phrase for status code. */
const char *cjang_plus_status_reason(int status_code);

/* 7. Builds JSON HTTP/1.1 response into fixed buffer. Returns byte length. */
size_t cjang_plus_build_json_resp(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *json_body
);

/* 8. Finds index of matching method + path in routes table, or -1 if not found. */
int cjang_plus_route_match_idx(
    const cjang_route_t *routes,
    size_t route_count,
    const char *method,
    const char *path
);

/* 9. Returns 1 if path exists with given method in routes table, else 0. */
int cjang_plus_path_has_allowed_method(
    const cjang_route_t *routes,
    size_t route_count,
    const char *path,
    const char *method
);

/* 10. Returns elapsed microseconds since start_ts using CLOCK_MONOTONIC. */
uint64_t cjang_plus_elapsed_us_since(const struct timespec *start_ts);

#endif
