#include "cj_portal_router_v19.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_portal_note_hist_update_v1.h"
#include "cj_portal_router_v18.h"
#include "cj_http_status_v1.h"
#include <stdlib.h>
#include <string.h>

static int parse_update_fields(
    const char *req_buf,
    size_t *out_idx,
    char *out_text,
    size_t out_text_cap
) {
    if (!req_buf || !out_idx || !out_text || out_text_cap == 0) return -1;

    char raw_idx[32];
    raw_idx[0] = '\0';
    out_text[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int got_idx = 0;
    int got_txt = 0;

    if (body && blen > 0) {
        if (cj_http_get_form_field(body, "index", raw_idx, sizeof(raw_idx)) == 0) got_idx = 1;
        if (cj_http_get_form_field(body, "text", out_text, out_text_cap) == 0) got_txt = 1;
    }

    if (!got_idx && cj_http_get_query_param(req_buf, "index", raw_idx, sizeof(raw_idx)) == 0) {
        got_idx = 1;
    }
    if (!got_txt && cj_http_get_query_param(req_buf, "text", out_text, out_text_cap) == 0) {
        got_txt = 1;
    }

    if (!got_idx || !got_txt || raw_idx[0] == '\0') return -1;
    for (size_t i = 0; raw_idx[i] != '\0'; i++) {
        if (raw_idx[i] < '0' || raw_idx[i] > '9') return -1;
    }

    *out_idx = (size_t)strtoull(raw_idx, NULL, 10);
    return 0;
}

size_t cj_portal_route_v19(
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

    if (strncmp(target, "/note/history/update", 20) == 0 &&
        (target[20] == '\0' || target[20] == '?')) {
        if (strcmp(method, "POST") != 0 && strcmp(method, "PUT") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        size_t idx = 0;
        char text[CJ_NOTE_HIST_ITEM_MAX];
        if (parse_update_fields(req_buf, &idx, text, sizeof(text)) != 0) {
            return cj_http_build_400(resp_buf, resp_cap);
        }
        return cj_portal_build_note_hist_update_resp(idx, text, resp_buf, resp_cap);
    }

    return cj_portal_route_v18(root_dir, req_buf, resp_buf, resp_cap);
}
