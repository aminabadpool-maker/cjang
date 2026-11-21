#include "cj_portal_router_v16.h"
#include "cj_http_req_v1.h"
#include "cj_http_query_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_portal_note_hist_prefix_v1.h"
#include "cj_portal_router_v15.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v16(
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

    if (strncmp(target, "/note/history/prefix", 20) == 0 &&
        (target[20] == '\0' || target[20] == '?')) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        char prefix[CJ_NOTE_HIST_ITEM_MAX];
        prefix[0] = '\0';
        (void)cj_http_get_query_param(req_buf, "prefix", prefix, sizeof(prefix));
        return cj_portal_build_note_hist_prefix_resp(prefix, resp_buf, resp_cap);
    }

    return cj_portal_route_v15(root_dir, req_buf, resp_buf, resp_cap);
}
