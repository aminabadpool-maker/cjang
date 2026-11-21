#include "cj_portal_router_v14.h"
#include "cj_http_req_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_portal_note_hist_persist_v1.h"
#include "cj_portal_router_v13.h"
#include "cj_http_status_v1.h"
#include <string.h>

#define CJ_PERSIST_HIST_PATH "build/cjang_note_hist.txt"

size_t cj_portal_route_v14(
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

    if (strcmp(target, "/note/history/persist") == 0) {
        if (strcmp(method, "GET") == 0) {
            return cj_portal_build_note_hist_persist_resp(CJ_PERSIST_HIST_PATH, resp_buf, resp_cap);
        }
        if (strcmp(method, "POST") == 0) {
            if (cj_portal_note_hist_save_file(CJ_PERSIST_HIST_PATH) != 0) {
                return cj_http_build_text_resp(
                    500,
                    "Internal Server Error",
                    "text/plain; charset=utf-8",
                    "persist failed",
                    resp_buf,
                    resp_cap
                );
            }
            return cj_portal_build_note_hist_persist_resp(CJ_PERSIST_HIST_PATH, resp_buf, resp_cap);
        }
        return cj_http_build_405(resp_buf, resp_cap);
    }

    return cj_portal_route_v13(root_dir, req_buf, resp_buf, resp_cap);
}
