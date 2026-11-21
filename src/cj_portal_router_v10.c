#include "cj_portal_router_v10.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_store_v1.h"
#include "cj_portal_router_v9.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v10(
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

    if (strcmp(target, "/note/last") == 0) {
        if (strcmp(method, "GET") == 0) {
            return cj_portal_build_note_last_resp(resp_buf, resp_cap);
        }
        if (strcmp(method, "POST") == 0) {
            char note[CJ_NOTE_STORE_MAX];
            note[0] = '\0';
            size_t blen = 0;
            const char *body = cj_http_find_body(req_buf, &blen);
            if (body && blen > 0 && cj_http_get_form_field(body, "text", note, sizeof(note)) == 0) {
                cj_portal_note_store_set(note);
            } else if (cj_http_get_query_param(req_buf, "text", note, sizeof(note)) == 0) {
                cj_portal_note_store_set(note);
            }
            return cj_portal_build_note_last_resp(resp_buf, resp_cap);
        }
        return cj_http_build_405(resp_buf, resp_cap);
    }

    return cj_portal_route_v9(root_dir, req_buf, resp_buf, resp_cap);
}
