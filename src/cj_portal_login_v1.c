#include "cj_portal_login_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_portal_session_v1.h"
#include "cj_http_resp_v1.h"
#include <string.h>

size_t cj_portal_handle_login(
    const char *req_buf,
    const char *cookie_name,
    const char *session_val,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char user[128];
    user[0] = '\0';

    int got = 0;
    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    if (body && blen > 0 && cj_http_get_form_field(body, "user", user, sizeof(user)) == 0) {
        got = 1;
    } else if (cj_http_get_query_param(req_buf, "user", user, sizeof(user)) == 0) {
        got = 1;
    }

    if (!got || strcmp(user, "admin") != 0) {
        return cj_http_build_text_resp(
            401,
            "Unauthorized",
            "text/plain; charset=utf-8",
            "unauthorized",
            out_buf,
            out_cap
        );
    }

    return cj_portal_build_session_resp(
        cookie_name,
        session_val,
        "logged in: admin",
        out_buf,
        out_cap
    );
}
