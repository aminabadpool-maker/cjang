#include "cj_portal_router_v9.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r_h = "GET /health HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n_h = cj_portal_route_v9("public", r_h, resp, sizeof(resp));
    assert(n_h > 0);
    assert(strstr(resp, "{\"status\":\"ok\",\"session\":\"active\",\"engine\":\"cjang-c11\"}") != NULL);

    const char *r_e = "GET /echo?msg=v22 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_e = cj_portal_route_v9("public", r_e, resp, sizeof(resp));
    assert(n_e > 0);
    assert(strstr(resp, "{\"echo\":\"v22\"}") != NULL);

    const char *r_html = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_html = cj_portal_route_v9("public", r_html, resp, sizeof(resp));
    assert(n_html > 0);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    printf("OK: cj_portal_router_v9 passed\n");
    return 0;
}
