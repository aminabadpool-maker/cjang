#include "cj_portal_router_v34.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_hist_prepend_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_portal_router_v33.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v34(
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

    if (strncmp(target, "/note/history/prepend", 21) == 0 &&
        (target[21] == '\0' || target[21] == '?')) {
        if (strcmp(method, "POST") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }

        char val[CJ_NOTE_HIST_ITEM_MAX];
        val[0] = '\0';

        size_t blen = 0;
        const char *body = cj_http_find_body(req_buf, &blen);
        int found = 0;

        if (body && blen > 0) {
            if (cj_http_get_form_field(body, "text", val, sizeof(val)) == 0 && val[0] != '\0') {
                found = 1;
            } else if (cj_http_get_form_field(body, "q", val, sizeof(val)) == 0 && val[0] != '\0') {
                found = 1;
            }
        }

        if (!found) {
            if (cj_http_get_query_param(req_buf, "text", val, sizeof(val)) == 0 && val[0] != '\0') {
                found = 1;
            } else if (cj_http_get_query_param(req_buf, "q", val, sizeof(val)) == 0 && val[0] != '\0') {
                found = 1;
            }
        }

        return cj_portal_build_note_hist_prepend_resp(found ? val : "", resp_buf, resp_cap);
    }

    return cj_portal_route_v33(root_dir, req_buf, resp_buf, resp_cap);
}
