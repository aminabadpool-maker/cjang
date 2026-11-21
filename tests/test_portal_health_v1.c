#include "cj_portal_health_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];

    const char *r1 = "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n1 = cj_portal_build_health_resp(r1, "cj_sid", "node42", resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "{\"status\":\"ok\",\"session\":\"none\",\"engine\":\"cjang-c11\"}") != NULL);

    const char *r2 = "GET /health HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    size_t n2 = cj_portal_build_health_resp(r2, "cj_sid", "node42", resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "{\"status\":\"ok\",\"session\":\"active\",\"engine\":\"cjang-c11\"}") != NULL);

    printf("OK: cj_portal_health_v1 passed\n");
    return 0;
}
