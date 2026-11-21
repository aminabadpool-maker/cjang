#include "cj_portal_router_v36.h"
#include "cj_note_db_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_note_db_init();
    char resp[CJ_HTTP_RESP_MAX];

    const char *create_req =
        "POST /note HTTP/1.1\r\nHost: localhost\r\n\r\ntitle=Guide&body=C11+db";
    assert(cj_portal_route_v36("public", create_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"id\":1") != NULL);

    const char *list_req =
        "GET /notes HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v36("public", list_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"count\":1") != NULL);

    const char *upd_req =
        "POST /note/update HTTP/1.1\r\nHost: localhost\r\n\r\nid=1&title=Guide+v2";
    assert(cj_portal_route_v36("public", upd_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"title\":\"Guide v2\"") != NULL);

    const char *del_req =
        "POST /note/delete HTTP/1.1\r\nHost: localhost\r\n\r\nid=1";
    assert(cj_portal_route_v36("public", del_req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "\"count\":0") != NULL);

    printf("OK: cj_portal_router_v36 passed\n");
    return 0;
}
