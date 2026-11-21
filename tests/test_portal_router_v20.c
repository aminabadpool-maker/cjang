#include "cj_portal_router_v20.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=first_item";
    assert(cj_portal_route_v20("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=second_item";
    assert(cj_portal_route_v20("public", p2, resp, sizeof(resp)) > 0);

    const char *g_item = "GET /note/history/item?index=1 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v20("public", g_item, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"found\":true,\"index\":1,\"text\":\"second_item\"}") != NULL);

    const char *g_miss = "GET /note/history/item?index=9 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v20("public", g_miss, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_router_v20 passed\n");
    return 0;
}
