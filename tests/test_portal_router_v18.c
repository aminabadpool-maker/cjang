#include "cj_portal_router_v18.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=a0";
    assert(cj_portal_route_v18("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=a1";
    assert(cj_portal_route_v18("public", p2, resp, sizeof(resp)) > 0);

    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=a2";
    assert(cj_portal_route_v18("public", p3, resp, sizeof(resp)) > 0);

    const char *d1 = "POST /note/history/delete HTTP/1.1\r\nHost: localhost\r\n\r\nindex=1";
    assert(cj_portal_route_v18("public", d1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"deleted\":true,\"index\":1,\"count\":2}") != NULL);

    const char *g1 = "GET /note/history HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v18("public", g1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"a0\",\"a2\"]}") != NULL);

    printf("OK: cj_portal_router_v18 passed\n");
    return 0;
}
