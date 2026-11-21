#include "cj_portal_router_v35.h"
#include "cj_comment_db_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_comment_db_init();
    char resp[CJ_HTTP_RESP_MAX];

    const char *add_req =
        "POST /comment HTTP/1.1\r\nHost: localhost\r\n\r\nnote_id=7&author=reza&text=nice+work";
    assert(cj_portal_route_v35("public", add_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"id\":1") != NULL);

    const char *mod_req =
        "POST /comment/moderate HTTP/1.1\r\nHost: localhost\r\n\r\nid=1&status=1";
    assert(cj_portal_route_v35("public", mod_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"status\":1") != NULL);

    const char *list_req =
        "GET /comments?note_id=7 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v35("public", list_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"count\":1") != NULL);

    const char *del_req =
        "POST /comment/delete HTTP/1.1\r\nHost: localhost\r\n\r\nid=1";
    assert(cj_portal_route_v35("public", del_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"count\":0") != NULL);

    printf("OK: cj_portal_router_v35 passed\n");
    return 0;
}
