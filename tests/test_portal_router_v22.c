#include "cj_portal_router_v22.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=exp_one";
    assert(cj_portal_route_v22("public", p1, resp, sizeof(resp)) > 0);

    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=exp_two";
    assert(cj_portal_route_v22("public", p2, resp, sizeof(resp)) > 0);

    const char *g_exp = "GET /note/history/export HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v22("public", g_exp, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "Content-Disposition: attachment; filename=\"cjang_note_history.txt\"\r\n") != NULL);
    assert(strstr(resp, "\r\n\r\nexp_one\nexp_two\n") != NULL);

    printf("OK: cj_portal_router_v22 passed\n");
    return 0;
}
