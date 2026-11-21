#include "cj_portal_resp_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[16384];
    size_t n = cj_portal_build_http_200(
        "cjang",
        "Portal Ready",
        "Static and dynamic C11 modules active.",
        resp,
        sizeof(resp)
    );

    assert(n > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Content-Type: text/html; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "<h1>Portal Ready</h1>") != NULL);

    printf("OK: cj_portal_resp_v1 passed\n");
    return 0;
}
