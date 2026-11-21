#include "cj_portal_stats_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];

    const char *r_guest = "GET /stats HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n1 = cj_portal_build_stats_resp(r_guest, "cj_sid", "node42", 7u, resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "Content-Type: application/json; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "{\"role\":\"guest\",\"routes\":7,\"engine\":\"cjang-c11\"}") != NULL);

    const char *r_admin = "GET /stats HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n2 = cj_portal_build_stats_resp(r_admin, "cj_sid", "node42", 7u, resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "{\"role\":\"admin\",\"routes\":7,\"engine\":\"cjang-c11\"}") != NULL);

    printf("OK: cj_portal_stats_v1 passed\n");
    return 0;
}
