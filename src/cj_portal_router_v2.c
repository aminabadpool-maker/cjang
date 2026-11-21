#include "cj_portal_router_v2.h"
#include "cj_http_req_v1.h"
#include "cj_http_json_v1.h"
#include "cj_portal_dyn_v1.h"
#include "cj_http_dispatch_v1.h"
#include "cj_http_status_v1.h"
#include <string.h>

size_t cj_portal_route_v2(
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

    if (strcmp(target, "/health") == 0) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        return cj_http_build_json_kv(200, "OK", "status", "ok", resp_buf, resp_cap);
    }

    if (strncmp(target, "/note", 5) == 0 && (target[5] == '\0' || target[5] == '?')) {
        if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0) {
            return cj_http_build_405(resp_buf, resp_cap);
        }
        return cj_portal_handle_dynamic(req_buf, resp_buf, resp_cap);
    }

    return cj_http_dispatch_static(root_dir, req_buf, resp_buf, resp_cap);
}
