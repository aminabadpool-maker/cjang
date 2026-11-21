#include "cj_portal_router_v12.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=alpha";
    assert(cj_portal_route_v12("public", p1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":1,\"items\":[\"alpha\"]}") != NULL);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=beta";
    assert(cj_portal_route_v12("public", p2, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"alpha\",\"beta\"]}") != NULL);

    const char *g1 = "GET /note/history HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v12("public", g1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"alpha\",\"beta\"]}") != NULL);

    printf("OK: cj_portal_router_v12 passed\n");
    return 0;
}
