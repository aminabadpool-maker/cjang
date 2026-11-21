#include "cj_portal_router_v29.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=one";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=two";
    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=three";
    assert(cj_portal_route_v29("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v29("public", p2, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v29("public", p3, resp, sizeof(resp)) > 0);

    const char *post_trim = "POST /note/history/trim HTTP/1.1\r\nHost: localhost\r\n\r\nkeep=2";
    assert(cj_portal_route_v29("public", post_trim, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"trimmed\":1,\"count\":2}") != NULL);

    printf("OK: cj_portal_router_v29 passed\n");
    return 0;
}
