#include "cj_http_dispatch_v1.h"
#include "cj_http_req_v1.h"
#include "cj_http_static_v1.h"
#include <string.h>

size_t cj_http_dispatch_static(
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

    if (strcmp(method, "GET") != 0 && strcmp(method, "HEAD") != 0) {
        return cj_http_build_404(resp_buf, resp_cap);
    }

    const char *rel = target;
    if (strcmp(target, "/") == 0) {
        rel = "index.html";
    } else if (target[0] == '/') {
        rel = target + 1;
    }

    size_t out_len = 0;
    if (cj_http_build_static_200(root_dir, rel, resp_buf, resp_cap, &out_len) != 0) {
        return cj_http_build_404(resp_buf, resp_cap);
    }

    return out_len;
}
