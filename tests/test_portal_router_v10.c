#include "cj_portal_router_v10.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r_post = "POST /note/last HTTP/1.1\r\nHost: localhost\r\n\r\ntext=saved+v23";
    size_t n1 = cj_portal_route_v10("public", r_post, resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "{\"last_note\":\"saved v23\"}") != NULL);

    const char *r_get = "GET /note/last HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n2 = cj_portal_route_v10("public", r_get, resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "{\"last_note\":\"saved v23\"}") != NULL);

    const char *r_h = "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n3 = cj_portal_route_v10("public", r_h, resp, sizeof(resp));
    assert(n3 > 0);
    assert(strstr(resp, "{\"status\":\"ok\"") != NULL);

    printf("OK: cj_portal_router_v10 passed\n");
    return 0;
}
