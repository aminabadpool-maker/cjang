#include "cj_portal_dashboard_v1.h"
#include "cj_portal_auth_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_build_dashboard_resp(
    const char *req_buf,
    const char *cookie_name,
    const char *expected_sid,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    int authed = cj_portal_has_cookie_val(req_buf, cookie_name, expected_sid);
    const char *state = authed ? "authenticated" : "guest";

    char html[2048];
    int n = snprintf(
        html,
        sizeof(html),
        "<!doctype html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"utf-8\" />\n"
        "  <title>cjang dashboard</title>\n"
        "  <link rel=\"stylesheet\" href=\"/style.css\" />\n"
        "</head>\n"
        "<body>\n"
        "  <h1>cjang Portal Dashboard</h1>\n"
        "  <p id=\"session-state\">state: %s</p>\n"
        "  <nav>\n"
        "    <a href=\"/health\">health</a> |\n"
        "    <a href=\"/note?text=hello\">note</a> |\n"
        "    <a href=\"/session\">session</a> |\n"
        "    <a href=\"/logout\">logout</a>\n"
        "  </nav>\n"
        "</body>\n"
        "</html>\n",
        state
    );
    if (n <= 0 || (size_t)n >= sizeof(html)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "text/html; charset=utf-8",
        html,
        out_buf,
        out_cap
    );
}
