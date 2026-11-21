#include "cj_portal_router_v5.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r_in = "POST /login HTTP/1.1\r\nHost: localhost\r\n\r\nuser=admin";
    size_t n_in = cj_portal_route_v5("public", r_in, resp, sizeof(resp));
    assert(n_in > 0);
    assert(strstr(resp, "Set-Cookie: cj_sid=node42; Path=/; HttpOnly\r\n") != NULL);
    assert(strstr(resp, "logged in: admin") != NULL);

    const char *r_out = "POST /logout HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_out = cj_portal_route_v5("public", r_out, resp, sizeof(resp));
    assert(n_out > 0);
    assert(strstr(resp, "Max-Age=0") != NULL);

    const char *r_html = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_html = cj_portal_route_v5("public", r_html, resp, sizeof(resp));
    assert(n_html > 0);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    printf("OK: cj_portal_router_v5 passed\n");
    return 0;
}
