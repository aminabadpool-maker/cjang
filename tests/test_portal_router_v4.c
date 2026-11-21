#include "cj_portal_router_v4.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r_out = "POST /logout HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_out = cj_portal_route_v4("public", r_out, resp, sizeof(resp));
    assert(n_out > 0);
    assert(strstr(resp, "Max-Age=0") != NULL);
    assert(strstr(resp, "logged out") != NULL);

    const char *r_ses = "GET /session HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n_ses = cj_portal_route_v4("public", r_ses, resp, sizeof(resp));
    assert(n_ses > 0);
    assert(strstr(resp, "authenticated: node42") != NULL);

    const char *r_html = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_html = cj_portal_route_v4("public", r_html, resp, sizeof(resp));
    assert(n_html > 0);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    printf("OK: cj_portal_router_v4 passed\n");
    return 0;
}
