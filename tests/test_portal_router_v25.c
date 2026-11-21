#include "cj_portal_router_v25.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=p0";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=p1";
    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=p2";
    assert(cj_portal_route_v25("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v25("public", p2, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v25("public", p3, resp, sizeof(resp)) > 0);

    const char *g_pg = "GET /note/history/page?offset=1&limit=1 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v25("public", g_pg, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"total\":3,\"offset\":1,\"limit\":1,\"count\":1,\"items\":[\"p1\"]}") != NULL);

    printf("OK: cj_portal_router_v25 passed\n");
    return 0;
}
