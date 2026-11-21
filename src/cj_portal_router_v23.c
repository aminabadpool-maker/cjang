#include "cj_portal_router_v23.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_hist_import_v1.h"
#include "cj_portal_router_v22.h"
#include "cj_http_status_v1.h"
#include <string.h>

#ifndef CJ_IMPORT_BUF_MAX
#define CJ_IMPORT_BUF_MAX 2048u
#endif

static int extract_import_payload(const char *req_buf, char *out_buf, size_t out_cap, int *out_clear) {
    if (!req_buf || !out_buf || out_cap == 0 || !out_clear) return -1;
    out_buf[0] = '\0';
    *out_clear = 0;

    char clr_raw[16];
    clr_raw[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found_text = 0;

    if (body && blen > 0) {
        if (cj_http_get_form_field(body, "text", out_buf, out_cap) == 0) {
            found_text = 1;
        } else if (cj_http_get_form_field(body, "data", out_buf, out_cap) == 0) {
            found_text = 1;
        }
        if (cj_http_get_form_field(body, "clear", clr_raw, sizeof(clr_raw)) == 0) {
            if (strcmp(clr_raw, "1") == 0 || strcmp(clr_raw, "true") == 0) {
                *out_clear = 1;
            }
        }
    }

    if (!found_text) {
        if (cj_http_get_query_param(req_buf, "text", out_buf, out_cap) == 0) {
            found_text = 1;
        } else if (cj_http_get_query_param(req_buf, "data", out_buf, out_cap) == 0) {
            found_text = 1;
        }
    }

    if (clr_raw[0] == '\0' && cj_http_get_query_param(req_buf, "clear", clr_raw, sizeof(clr_raw)) == 0) {
        if (strcmp(clr_raw, "1") == 0 || strcmp(clr_raw, "true") == 0) {
            *out_clear = 1;
        }
    }

    return found_text ? 0 : -1;
}

size_t cj_portal_route_v23(
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

    if (strncmp(target, "/note/history/import", 20) == 0 &&
        (target[20] == '\0' || target[20] == '?')) {
        if (strcmp(method, "POST") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        char payload[CJ_IMPORT_BUF_MAX];
        int clear_first = 0;
        if (extract_import_payload(req_buf, payload, sizeof(payload), &clear_first) != 0) {
            return cj_http_build_400(resp_buf, resp_cap);
        }
        return cj_portal_build_note_hist_import_resp(payload, clear_first, resp_buf, resp_cap);
    }

    return cj_portal_route_v22(root_dir, req_buf, resp_buf, resp_cap);
}
