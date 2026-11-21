#include "cj_portal_router_v32.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=n_a";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=n_b";
    assert(cj_portal_route_v32("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v32("public", p2, resp, sizeof(resp)) > 0);

    const char *post_pop = "POST /note/history/pop HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v32("public", post_pop, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"popped\":true,\"item\":\"n_b\",\"count\":1}") != NULL);

    printf("OK: cj_portal_router_v32 passed\n");
    return 0;
}
