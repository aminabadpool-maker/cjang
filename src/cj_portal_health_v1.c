#include "cj_portal_health_v1.h"
#include "cj_portal_auth_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_health_resp(
    const char *req_buf,
    const char *cookie_name,
    const char *expected_sid,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    int authed = cj_portal_has_cookie_val(req_buf, cookie_name, expected_sid);
    const char *sess = authed ? "active" : "none";

    char json[256];
    int n = snprintf(
        json,
        sizeof(json),
        "{\"status\":\"ok\",\"session\":\"%s\",\"engine\":\"cjang-c11\"}",
        sess
    );
    if (n <= 0 || (size_t)n >= sizeof(json)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        json,
        out_buf,
        out_cap
    );
}
