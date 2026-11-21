#include "cj_portal_router_v31.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_hist_move_v1.h"
#include "cj_portal_router_v30.h"
#include "cj_http_status_v1.h"
#include <stdlib.h>
#include <string.h>

static int parse_move_idx(const char *req_buf, const char *key, size_t *out_idx) {
    if (!req_buf || !key || !out_idx) return -1;

    char raw[32];
    raw[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found = 0;

    if (body && blen > 0 && cj_http_get_form_field(body, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    } else if (cj_http_get_query_param(req_buf, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    }

    if (!found) return -1;

    for (size_t i = 0; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_idx = (size_t)strtoull(raw, NULL, 10);
    return 0;
}

size_t cj_portal_route_v31(
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

    if (strncmp(target, "/note/history/move", 18) == 0 &&
        (target[18] == '\0' || target[18] == '?')) {
        if (strcmp(method, "POST") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        size_t idx_from = 0;
        size_t idx_to = 0;
        if (parse_move_idx(req_buf, "from", &idx_from) != 0 ||
            parse_move_idx(req_buf, "to", &idx_to) != 0) {
            return cj_http_build_400(resp_buf, resp_cap);
        }
        return cj_portal_build_note_hist_move_resp(idx_from, idx_to, resp_buf, resp_cap);
    }

    return cj_portal_route_v30(root_dir, req_buf, resp_buf, resp_cap);
}
