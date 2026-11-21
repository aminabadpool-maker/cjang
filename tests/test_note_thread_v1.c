#include "cj_note_thread_v1.h"
#include "cj_note_db_v1.h"
#include "cj_comment_db_v1.h"
#include "cj_http_static_v1.h"
#include "cj_portal_router_v37.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_note_db_init();
    cj_comment_db_init();

    unsigned int nid = 0, cid1 = 0, cid2 = 0;
    assert(cj_note_db_create("Thread Title", "Main note content", &nid) == 0);
    assert(cj_comment_db_add(nid, "ali", "first reply", &cid1) == 0);
    assert(cj_comment_db_add(nid, "sara", "second reply", &cid2) == 0);

    assert(cj_note_thread_count_comments(nid) == 2);

    char out_json[4096];
    assert(cj_note_thread_format_json(nid, out_json, sizeof(out_json)) > 0);
    assert(strstr(out_json, "\"title\":\"Thread Title\"") != NULL);
    assert(strstr(out_json, "\"comment_count\":2") != NULL);
    assert(strstr(out_json, "first reply") != NULL);

    char resp[CJ_HTTP_RESP_MAX];
    const char *req = "GET /note/thread?id=1 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v37("public", req, resp, sizeof(resp)) > 0);
    assert(strstr(resp, "200 OK") != NULL);
    assert(strstr(resp, "\"comment_count\":2") != NULL);

    printf("OK: cj_note_thread_v1 passed\n");
    return 0;
}
