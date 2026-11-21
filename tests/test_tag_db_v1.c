#include "cj_tag_db_v1.h"
#include "cj_portal_router_v38.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *db_path = "build/cj_tag_test.bin";
    remove(db_path);

    cj_tag_db_init();
    unsigned int t1 = 0u, t2 = 0u;
    assert(cj_tag_db_add(3u, "c11", &t1) == 0);
    assert(cj_tag_db_add(3u, "backend", &t2) == 0);
    assert(cj_tag_db_count() == 2u);
    assert(cj_tag_db_count_by_note(3u) == 2u);

    assert(cj_tag_db_save(db_path) == 0);
    cj_tag_db_init();
    assert(cj_tag_db_count() == 0u);
    assert(cj_tag_db_load(db_path) == 0);
    assert(cj_tag_db_count() == 2u);

    char resp[CJ_HTTP_RESP_MAX];
    const char *list_req = "GET /tags?note_id=3 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_portal_route_v38("public", list_req, resp, sizeof(resp)) > 0u);
    assert(strstr(resp, "\"count\":2") != NULL);

    assert(cj_tag_db_delete(t2) == 0);
    assert(cj_tag_db_count() == 1u);

    remove(db_path);
    printf("OK: cj_tag_db_v1 passed\n");
    return 0;
}
