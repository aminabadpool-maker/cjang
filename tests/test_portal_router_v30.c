#include "cj_portal_router_v30.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=first_sw";
    const char *p2 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=second_sw";
    assert(cj_portal_route_v30("public", p1, resp, sizeof(resp)) > 0);
    assert(cj_portal_route_v30("public", p2, resp, sizeof(resp)) > 0);

    const char *post_swap = "POST /note/history/swap HTTP/1.1\r\nHost: localhost\r\n\r\na=0&b=1";
    assert(cj_portal_route_v30("public", post_swap, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"swapped\":true,\"a\":0,\"b\":1}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 9);
    assert(strcmp(item, "second_sw") == 0);

    printf("OK: cj_portal_router_v30 passed\n");
    return 0;
}
