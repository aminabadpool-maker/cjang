#include "cj_portal_router_v20.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_portal_note_hist_item_v1.h"
#include "cj_portal_router_v19.h"
#include "cj_http_status_v1.h"
#include <stdlib.h>
#include <string.h>

static int parse_item_index(const char *req_buf, size_t *out_idx) {
    if (!req_buf || !out_idx) return -1;

    char raw[32];
    raw[0] = '\0';
    if (cj_http_get_query_param(req_buf, "index", raw, sizeof(raw)) != 0 || raw[0] == '\0') {
        return -1;
    }

    for (size_t i = 0; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_idx = (size_t)strtoull(raw, NULL, 10);
    return 0;
}

size_t cj_portal_route_v20(
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

    if (strncmp(target, "/note/history/item", 18) == 0 &&
        (target[18] == '\0' || target[18] == '?')) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        size_t idx = 0;
        if (parse_item_index(req_buf, &idx) != 0) {
            return cj_http_build_400(resp_buf, resp_cap);
        }
        return cj_portal_build_note_hist_item_resp(idx, resp_buf, resp_cap);
    }

    return cj_portal_route_v19(root_dir, req_buf, resp_buf, resp_cap);
}
