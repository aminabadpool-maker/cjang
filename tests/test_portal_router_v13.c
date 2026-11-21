#include "cj_portal_router_v13.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=one";
    assert(cj_portal_route_v13("public", p1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":1,\"items\":[\"one\"]}") != NULL);

    const char *clr = "POST /note/history/clear HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v13("public", clr, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"cleared\":true,\"count\":0}") != NULL);

    const char *g1 = "GET /note/history HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v13("public", g1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":0,\"items\":[]}") != NULL);

    printf("OK: cj_portal_router_v13 passed\n");
    return 0;
}
