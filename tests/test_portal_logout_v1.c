#include "cj_portal_logout_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];
    size_t len = cj_portal_build_logout_resp("cj_sid", "session cleared", resp, sizeof(resp));

    assert(len > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Set-Cookie: cj_sid=; Path=/; Max-Age=0; HttpOnly\r\n") != NULL);
    assert(strstr(resp, "Content-Length: 15\r\n") != NULL);
    assert(strstr(resp, "session cleared") != NULL);

    printf("OK: cj_portal_logout_v1 passed\n");
    return 0;
}
