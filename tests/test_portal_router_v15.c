#include "cj_portal_router_v15.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=cjang+core";
    assert(cj_portal_route_v15("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=cjang+router";
    assert(cj_portal_route_v15("public", p2, resp, sizeof(resp)) > 0);

    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=other+item";
    assert(cj_portal_route_v15("public", p3, resp, sizeof(resp)) > 0);

    const char *s1 = "GET /note/history/search?q=cjang HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v15("public", s1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"query\":\"cjang\",\"count\":2,\"items\":[\"cjang core\",\"cjang router\"]}") != NULL);

    printf("OK: cj_portal_router_v15 passed\n");
    return 0;
}
