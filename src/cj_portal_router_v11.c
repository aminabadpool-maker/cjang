#include "cj_portal_router_v11.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_body_v1.h"
#include "cj_portal_note_store_v1.h"
#include "cj_portal_note_persist_v1.h"
#include "cj_portal_router_v10.h"
#include "cj_http_status_v1.h"
#include <string.h>

#define CJ_PERSIST_NOTE_PATH "build/cjang_last_note.txt"

size_t cj_portal_route_v11(
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

    if (strcmp(target, "/note/persist") == 0) {
        if (strcmp(method, "GET") == 0) {
            return cj_portal_build_note_persist_resp(CJ_PERSIST_NOTE_PATH, resp_buf, resp_cap);
        }
        if (strcmp(method, "POST") == 0) {
            char note[CJ_NOTE_STORE_MAX];
            note[0] = '\0';
            size_t blen = 0;
            const char *body = cj_http_find_body(req_buf, &blen);
            if (body && blen > 0 && cj_http_get_form_field(body, "text", note, sizeof(note)) == 0) {
                cj_portal_note_save_file(CJ_PERSIST_NOTE_PATH, note);
            } else if (cj_http_get_query_param(req_buf, "text", note, sizeof(note)) == 0) {
                cj_portal_note_save_file(CJ_PERSIST_NOTE_PATH, note);
            }
            return cj_portal_build_note_persist_resp(CJ_PERSIST_NOTE_PATH, resp_buf, resp_cap);
        }
        return cj_http_build_405(resp_buf, resp_cap);
    }

    return cj_portal_route_v10(root_dir, req_buf, resp_buf, resp_cap);
}
