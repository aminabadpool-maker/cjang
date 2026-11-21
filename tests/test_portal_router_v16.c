#include "cj_portal_router_v16.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=dbg_one";
    assert(cj_portal_route_v16("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=dbg_two";
    assert(cj_portal_route_v16("public", p2, resp, sizeof(resp)) > 0);

    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=app_one";
    assert(cj_portal_route_v16("public", p3, resp, sizeof(resp)) > 0);

    const char *f1 = "GET /note/history/prefix?prefix=dbg_ HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v16("public", f1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"prefix\":\"dbg_\",\"count\":2,\"items\":[\"dbg_one\",\"dbg_two\"]}") != NULL);

    printf("OK: cj_portal_router_v16 passed\n");
    return 0;
}
