#include "cj_portal_dyn_v1.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_resp_v1.h"
#include "cj_http_static_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_handle_dynamic(
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
) {
    if (!resp_buf || resp_cap == 0) return 0;

    char method[CJ_HTTP_METHOD_MAX];
    char target[CJ_HTTP_TARGET_MAX];
    if (cj_http_parse_req_line(req_buf, method, sizeof(method), target, sizeof(target)) != 0) {
        return cj_http_build_404(resp_buf, resp_cap);
    }

    char msg[256];
    msg[0] = '\0';

    if (strcmp(target, "/note") == 0 && strcmp(method, "GET") == 0) {
        if (cj_http_get_query_param(req_buf, "text", msg, sizeof(msg)) != 0) {
            snprintf(msg, sizeof(msg), "empty query");
        }
        return cj_portal_build_http_200("cjang note", "Portal Note", msg, resp_buf, resp_cap);
    }

    if (strcmp(target, "/note") == 0 && strcmp(method, "POST") == 0) {
        size_t blen = 0;
        const char *body = cj_http_find_body(req_buf, &blen);
        if (!body || blen == 0 || cj_http_get_form_field(body, "text", msg, sizeof(msg)) != 0) {
            snprintf(msg, sizeof(msg), "empty body");
        }
        return cj_portal_build_http_200("cjang note", "Submitted Note", msg, resp_buf, resp_cap);
    }

    return cj_http_build_404(resp_buf, resp_cap);
}
