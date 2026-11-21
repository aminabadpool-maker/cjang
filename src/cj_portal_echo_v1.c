#include "cj_portal_echo_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_json_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_handle_echo(
    const char *req_buf,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char msg[256];
    msg[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    if (body && blen > 0 && cj_http_get_form_field(body, "msg", msg, sizeof(msg)) == 0) {
        return cj_http_build_json_kv(200, "OK", "echo", msg, out_buf, out_cap);
    }

    if (cj_http_get_query_param(req_buf, "msg", msg, sizeof(msg)) == 0) {
        return cj_http_build_json_kv(200, "OK", "echo", msg, out_buf, out_cap);
    }

    snprintf(msg, sizeof(msg), "");
    return cj_http_build_json_kv(200, "OK", "echo", msg, out_buf, out_cap);
}
