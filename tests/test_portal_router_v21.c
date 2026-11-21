#include "cj_portal_router_v21.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=item_one";
    assert(cj_portal_route_v21("public", p1, resp, sizeof(resp)) > 0);

    const char *g_cnt = "GET /note/history/count HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v21("public", g_cnt, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":1,\"max_slots\":8}") != NULL);

    printf("OK: cj_portal_router_v21 passed\n");
    return 0;
}
