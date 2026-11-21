#include "cj_portal_router_v2.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    size_t n_bad = cj_portal_route_v2("public", "INVALID", resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 400 Bad Request\r\n") != NULL);

    size_t n_mthd = cj_portal_route_v2("public", "DELETE /health HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_mthd > 0);
    assert(strstr(resp, "HTTP/1.1 405 Method Not Allowed\r\n") != NULL);

    size_t n_note = cj_portal_route_v2("public", "GET /note?text=v13+ok HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_note > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "<p>v13 ok</p>") != NULL);

    size_t n_css = cj_portal_route_v2("public", "GET /style.css HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_css > 0);
    assert(strstr(resp, "Content-Type: text/css; charset=utf-8\r\n") != NULL);

    printf("OK: cj_portal_router_v2 passed\n");
    return 0;
}
