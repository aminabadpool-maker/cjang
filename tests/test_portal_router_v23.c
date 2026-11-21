#include "cj_portal_router_v23.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *imp1 = "POST /note/history/import HTTP/1.1\r\nHost: localhost\r\n\r\ntext=first%0Asecond&clear=1";
    assert(cj_portal_route_v23("public", imp1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"imported\":2,\"count\":2}") != NULL);

    const char *g_hist = "GET /note/history HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v23("public", g_hist, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"first\",\"second\"]}") != NULL);

    printf("OK: cj_portal_router_v23 passed\n");
    return 0;
}
