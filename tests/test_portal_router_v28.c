#include "cj_portal_router_v28.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=same";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=diff";
    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=same";
    assert(cj_portal_route_v28("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v28("public", p2, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v28("public", p3, resp, sizeof(resp)) > 0);

    const char *post_dedup = "POST /note/history/dedup HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v28("public", post_dedup, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"removed\":1,\"count\":2}") != NULL);

    printf("OK: cj_portal_router_v28 passed\n");
    return 0;
}
