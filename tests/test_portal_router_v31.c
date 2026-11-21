#include "cj_portal_router_v31.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=a_mv";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=b_mv";
    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=c_mv";
    assert(cj_portal_route_v31("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v31("public", p2, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v31("public", p3, resp, sizeof(resp)) > 0);

    const char *post_move = "POST /note/history/move HTTP/1.1\r\nHost: localhost\r\n\r\nfrom=2&to=0";
    assert(cj_portal_route_v31("public", post_move, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"moved\":true,\"from\":2,\"to\":0}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 4);
    assert(strcmp(item, "c_mv") == 0);

    printf("OK: cj_portal_router_v31 passed\n");
    return 0;
}
