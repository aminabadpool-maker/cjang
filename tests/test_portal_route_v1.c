#include "cj_portal_route_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    size_t n_health = cj_portal_handle_request("public", "GET /health HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_health > 0);
    assert(strstr(resp, "Content-Type: application/json; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "{\"status\":\"ok\"}") != NULL);

    size_t n_index = cj_portal_handle_request("public", "GET / HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_index > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    size_t n_404 = cj_portal_handle_request("public", "GET /missing.html HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_404 > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_route_v1 passed\n");
    return 0;
}
