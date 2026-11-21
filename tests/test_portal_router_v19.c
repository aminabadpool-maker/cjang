#include "cj_portal_router_v19.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=n0";
    assert(cj_portal_route_v19("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=n1";
    assert(cj_portal_route_v19("public", p2, resp, sizeof(resp)) > 0);

    const char *u1 = "POST /note/history/update HTTP/1.1\r\nHost: localhost\r\n\r\nindex=1&text=n1_edited";
    assert(cj_portal_route_v19("public", u1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"updated\":true,\"index\":1,\"text\":\"n1_edited\",\"count\":2}") != NULL);

    const char *g1 = "GET /note/history HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v19("public", g1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"n0\",\"n1_edited\"]}") != NULL);

    printf("OK: cj_portal_router_v19 passed\n");
    return 0;
}
