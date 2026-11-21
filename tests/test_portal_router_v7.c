#include "cj_portal_router_v7.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    const char *r_stats = "GET /stats HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n_stats = cj_portal_route_v7("public", r_stats, resp, sizeof(resp));
    assert(n_stats > 0);
    assert(strstr(resp, "{\"role\":\"admin\",\"routes\":7,\"engine\":\"cjang-c11\"}") != NULL);

    const char *r_dash = "GET /dashboard HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_dash = cj_portal_route_v7("public", r_dash, resp, sizeof(resp));
    assert(n_dash > 0);
    assert(strstr(resp, "<title>cjang dashboard</title>") != NULL);

    const char *r_html = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n_html = cj_portal_route_v7("public", r_html, resp, sizeof(resp));
    assert(n_html > 0);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    printf("OK: cj_portal_router_v7 passed\n");
    return 0;
}
