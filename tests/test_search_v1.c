#include "cj_search_v1.h"
#include "cj_note_db_v1.h"
#include "cj_comment_db_v1.h"
#include "cj_portal_router_v39.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_note_db_init();
    cj_comment_db_init();

    unsigned int nid = 0u, cid = 0u;
    assert(cj_note_db_create("C11 Parser", "Zero-copy fixed buffer", &nid) == 0);
    assert(cj_comment_db_add(nid, "ali", "buffer is fast", &cid) == 0);

    size_t n_cnt = 0u, c_cnt = 0u;
    char out[4096];
    assert(cj_search_notes_json("buffer", out, sizeof(out), &n_cnt) > 0u);
    assert(n_cnt == 1u);

    assert(cj_search_comments_json("fast", out, sizeof(out), &c_cnt) > 0u);
    assert(c_cnt == 1u);

    char resp[CJ_HTTP_RESP_MAX];
    const char *req = "GET /search?q=buffer HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v39("public", req, resp, sizeof(resp)) > 0u);
    assert(strstr(resp, "200 OK") != NULL);
    assert(strstr(resp, "\"note_count\":1") != NULL);

    printf("OK: cj_search_v1 passed\n");
    return 0;
}
