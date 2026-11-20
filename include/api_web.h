#ifndef CJANG_API_WEB_H
#define CJANG_API_WEB_H

#include "api_codec.h"
#include "api_ext.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *key;
    const char *val;
} cjang_web_kv_t;

/* Checks method against allowed list (e.g. "GET,POST,HEAD").
 * If allowed, returns 1. If not allowed, builds HTTP 405 response with Allow header
 * into resp_out and returns 0. */
int cjang_web_check_method_or_405(
    const char *method,
    const char *allow_list,
    char *resp_out,
    size_t resp_cap
);

/* Extracts query or URL-encoded form parameter by key and URL-decodes value into out.
 * Returns decoded length (>=0) on success, or -1 if not found. */
int cjang_web_param_get_decoded(
    const char *kv_str,
    size_t kv_len,
    const char *key,
    char *out,
    size_t out_cap
);

/* Validates Content-Length against max_body_len.
 * Returns 0 if valid (writes parsed length to *out_len), or -1 if invalid/too large. */
int cjang_web_parse_content_length(
    const char *raw_req,
    size_t max_body_len,
    size_t *out_len
);

/* Renders {{key}} template using kv array.
 * If json_escape != 0, escapes special characters in replacement values for JSON safety.
 * Returns written length. */
size_t cjang_web_render_template_ex(
    const char *tmpl,
    const cjang_web_kv_t *vars,
    size_t var_count,
    int json_escape,
    char *out,
    size_t out_cap
);

/* Builds HTTP/1.1 response honoring HEAD (omits body while keeping Content-Length). */
size_t cjang_web_build_response(
    const char *method,
    int status_code,
    const char *reason,
    const char *content_type,
    const char *body,
    char *out,
    size_t out_cap
);

#endif
