#include "cj_portal_router_v15.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_portal_note_hist_search_v1.h"
#include "cj_portal_router_v14.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v15(
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

    if (strncmp(target, "/note/history/search", 20) == 0 &&
        (target[20] == '\0' || target[20] == '?')) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        char q[CJ_NOTE_HIST_ITEM_MAX];
        q[0] = '\0';
        (void)cj_http_get_query_param(req_buf, "q", q, sizeof(q));
        return cj_portal_build_note_hist_search_resp(q, resp_buf, resp_cap);
    }

    return cj_portal_route_v14(root_dir, req_buf, resp_buf, resp_cap);
}
