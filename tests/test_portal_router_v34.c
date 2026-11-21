#include "cj_portal_router_v34.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[CJ_HTTP_RESP_MAX];

    const char *p1 = "POST /note/history HTTP/1.1\r\nHost: localhost\r\n\r\ntext=tail_pr";
    assert(cj_portal_route_v34("public", p1, resp, sizeof(resp)) > 0);

    const char *post_pre = "POST /note/history/prepend HTTP/1.1\r\nHost: localhost\r\n\r\ntext=head_pr";
    assert(cj_portal_route_v34("public", post_pre, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "{\"prepended\":true,\"item\":\"head_pr\",\"count\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 7);
    assert(strcmp(item, "head_pr") == 0);

    printf("OK: cj_portal_router_v34 passed\n");
    return 0;
}
