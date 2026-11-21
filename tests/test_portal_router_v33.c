#include "cj_portal_router_v33.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=sh_a";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=sh_b";
    assert(cj_portal_route_v33("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v33("public", p2, resp, sizeof(resp)) > 0);

    const char *post_shift = "POST /note/history/shift HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v33("public", post_shift, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"shifted\":true,\"item\":\"sh_a\",\"count\":1}") != NULL);

    printf("OK: cj_portal_router_v33 passed\n");
    return 0;
}
