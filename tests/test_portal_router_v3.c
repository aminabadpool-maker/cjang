#include "cj_portal_router_v3.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r1 = "GET /session HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n1 = cj_portal_route_v3("public", r1, resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "Set-Cookie: cj_sid=node42; Path=/; HttpOnly\r\n") != NULL);
    assert(strstr(resp, "guest session issued") != NULL);

    const char *r2 = "GET /session HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n2 = cj_portal_route_v3("public", r2, resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "authenticated: node42") != NULL);

    const char *r3 = "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n3 = cj_portal_route_v3("public", r3, resp, sizeof(resp));
    assert(n3 > 0);
    assert(strstr(resp, "{\"status\":\"ok\"}") != NULL);

    printf("OK: cj_portal_router_v3 passed\n");
    return 0;
}
