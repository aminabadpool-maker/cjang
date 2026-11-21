#include "cj_portal_router_v39.h"
#include "cj_search_v1.h"
#include "cj_http_req_v1.h"
#include "cj_http_status_v1.h"
#include "cj_portal_router_v38.h"
#include <string.h>

static int match_search_path(const char *target, const char *path) {
    size_t plen = strlen(path);
    if (strncmp(target, path, plen) != 0) return 0;
    return (target[plen] == '\0' || target[plen] == '?');
}

size_t cj_portal_route_v39(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
) {
    if (!resp_buf || resp_cap == 0u) return 0u;

    char method[CJ_HTTP_METHOD_MAX];
    char target[CJ_HTTP_TARGET_MAX];
    if (cj_http_parse_req_line(req_buf, method, sizeof(method), target, sizeof(target)) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (match_search_path(target, "/search")) {
        if (strcmp(method, "GET") != 0) return cj_http_build_405(resp_buf, resp_cap);
        return cj_search_http_resp(req_buf, resp_buf, resp_cap);
    }

    return cj_portal_route_v38(root_dir, req_buf, resp_buf, resp_cap);
}
