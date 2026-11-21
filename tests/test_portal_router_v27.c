#include "cj_portal_router_v27.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=ping";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=ping";
    assert(cj_portal_route_v27("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v27("public", p2, resp, sizeof(resp)) > 0);

    const char *g_dup = "GET /note/history/dup?q=ping HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v27("public", g_dup, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"query\":\"ping\",\"exists\":true,\"matches\":2}") != NULL);

    printf("OK: cj_portal_router_v27 passed\n");
    return 0;
}
