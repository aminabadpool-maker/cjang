#include "cj_portal_route_v1.h"
#include "cj_http_req_v1.h"
#include "cj_http_dispatch_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_static_v1.h"
#include <string.h>

size_t cj_portal_handle_request(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
) {
    if (!resp_buf || resp_cap == 0) return 0;

    char method[CJ_HTTP_METHOD_MAX];
    char target[CJ_HTTP_TARGET_MAX];

    if (cj_http_parse_req_line(req_buf, method, sizeof(method), target, sizeof(target)) != 0) {
        return cj_http_build_404(resp_buf, resp_cap);
    }

    if (strcmp(target, "/health") == 0) {
        return cj_http_build_json_kv(200, "OK", "status", "ok", resp_buf, resp_cap);
    }

    return cj_http_dispatch_static(root_dir, req_buf, resp_buf, resp_cap);
}
