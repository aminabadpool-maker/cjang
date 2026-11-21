#include "cj_portal_router_v7.h"
#include "cj_http_req_v1.h"
#include "cj_portal_stats_v1.h"
#include "cj_portal_router_v6.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v7(
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

    if (strcmp(target, "/stats") == 0) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        return cj_portal_build_stats_resp(req_buf, "cj_sid", "node42", 7u, resp_buf, resp_cap);
    }

    return cj_portal_route_v6(root_dir, req_buf, resp_buf, resp_cap);
}
