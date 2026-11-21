#include "cj_portal_router_v17.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=job_ok";
    assert(cj_portal_route_v17("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=job_err";
    assert(cj_portal_route_v17("public", p2, resp, sizeof(resp)) > 0);

    const char *p3 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=sync_ok";
    assert(cj_portal_route_v17("public", p3, resp, sizeof(resp)) > 0);

    const char *f1 = "GET /note/history/suffix?suffix=_ok HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v17("public", f1, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"suffix\":\"_ok\",\"count\":2,\"items\":[\"job_ok\",\"sync_ok\"]}") != NULL);

    printf("OK: cj_portal_router_v17 passed\n");
    return 0;
}
