#include "cj_portal_router_v26.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=start_note";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=end_note";
    assert(cj_portal_route_v26("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v26("public", p2, resp, sizeof(resp)) > 0);

    const char *g_bnd = "GET /note/history/bounds HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v26("public", g_bnd, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"found\":true,\"count\":2,\"first\":\"start_note\",\"last\":\"end_note\"}") != NULL);

    printf("OK: cj_portal_router_v26 passed\n");
    return 0;
}
