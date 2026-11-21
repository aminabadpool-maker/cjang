#include "cj_portal_router_v24.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=r1";
    assert(cj_portal_route_v24("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=r2";
    assert(cj_portal_route_v24("public", p2, resp, sizeof(resp)) > 0);

    const char *g_rev = "GET /note/history/reverse HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v24("public", g_rev, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"r2\",\"r1\"]}") != NULL);

    printf("OK: cj_portal_router_v24 passed\n");
    return 0;
}
