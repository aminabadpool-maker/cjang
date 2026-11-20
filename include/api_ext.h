#ifndef CJANG_API_EXT_H
#define CJANG_API_EXT_H

#include "api_metrics.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t window_start_sec;
    uint32_t count_in_window;
    uint32_t max_per_window;
    uint64_t window_sec;
} cjang_ext_rate_t;

/* 1. Case-insensitive HTTP header lookup into fixed buffer. Returns value len or -1 if not found. */
int cjang_ext_header_get(const char *raw_req, const char *name, char *out, size_t out_cap);

/* 2. Extracts cookie value by name from cookie header string. Returns value len or -1 if not found. */
int cjang_ext_cookie_get(const char *cookie_hdr, const char *name, char *out, size_t out_cap);

/* 3. Returns 1 if method is allowed in comma/space-separated allow_list (e.g. "GET,POST"), else 0. */
int cjang_ext_method_allowed(const char *method, const char *allow_list);

/* 4. Cleans request path and rejects ".." traversal. Returns 0 on valid cleaned path, -1 if rejected. */
int cjang_ext_path_clean(const char *raw_path, char *out, size_t out_cap);

/* 5. Escapes string for safe JSON output into fixed buffer. Returns escaped length. */
size_t cjang_ext_json_escape(const char *src, char *dst, size_t dst_cap);

/* 6. Extracts int64 value for a simple flat JSON key. Returns 0 on success, -1 on failure. */
int cjang_ext_json_get_int64(const char *json, size_t json_len, const char *key, int64_t *out_val);

/* 7. Builds HTTP/1.1 response with optional X-Cjang-Us header into fixed buffer. Returns byte len. */
size_t cjang_ext_build_http_status(
    char *dst,
    size_t dst_cap,
    int status_code,
    const char *reason,
    const char *content_type,
    const char *body,
    uint64_t elapsed_us
);

/* 8. Fixed-window rate limiter using CLOCK_MONOTONIC. Returns 1 if allowed, 0 if rate-limited. */
void cjang_ext_rate_init(cjang_ext_rate_t *rl, uint32_t max_per_window, uint64_t window_sec);
int cjang_ext_rate_allow(cjang_ext_rate_t *rl);

/* 9. Parses "key=value" or "key: value" into fixed buffers. Returns 0 on success, -1 otherwise. */
int cjang_ext_parse_kv_line(
    const char *line,
    char *key_out,
    size_t key_cap,
    char *val_out,
    size_t val_cap
);

/* 10. Formats cjang_metrics_t into compact key=value text block. Returns written length. */
size_t cjang_ext_metrics_snapshot_kv(const cjang_metrics_t *m, char *out, size_t out_cap);

#endif
