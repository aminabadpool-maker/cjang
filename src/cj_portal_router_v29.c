#include "cj_portal_router_v29.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_hist_trim_v1.h"
#include "cj_portal_router_v28.h"
#include "cj_http_status_v1.h"
#include <stdlib.h>
#include <string.h>

static int parse_keep_val(const char *req_buf, size_t def_val, size_t *out_keep) {
    if (!req_buf || !out_keep) return -1;

    char raw[32];
    raw[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found = 0;

    if (body && blen > 0) {
        if (cj_http_get_form_field(body, "keep", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
            found = 1;
        } else if (cj_http_get_form_field(body, "limit", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
            found = 1;
        }
    }

    if (!found) {
        if (cj_http_get_query_param(req_buf, "keep", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
            found = 1;
        } else if (cj_http_get_query_param(req_buf, "limit", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
            found = 1;
        }
    }

    if (!found) {
        *out_keep = def_val;
        return 0;
    }

    for (size_t i = 0; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_keep = (size_t)strtoull(raw, NULL, 10);
    return 0;
}

size_t cj_portal_route_v29(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
) {
    if (!resp_buf || resp_cap == 0) return 0;

    char method[CJ_HTTP_METHOD_MAX];
    char target[CJ_HTTP_TARGET_MAX];
    if (cj_http_parse_req_line(req_buf, method, sizeof(method), target, sizeof(target)) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (strncmp(target, "/note/history/trim", 18) == 0 &&
        (target[18] == '\0' || target[18] == '?')) {
        if (strcmp(method, "POST") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        size_t keep = 10;
        if (parse_keep_val(req_buf, 10, &keep) != 0) {
            return cj_http_build_400(resp_buf, resp_cap);
        }
        return cj_portal_build_note_hist_trim_resp(keep, resp_buf, resp_cap);
    }

    return cj_portal_route_v28(root_dir, req_buf, resp_buf, resp_cap);
}
