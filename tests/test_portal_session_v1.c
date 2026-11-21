#include "cj_portal_session_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];
    const char *body = "session active";

    size_t len = cj_portal_build_session_resp("cj_sid", "node42", body, resp, sizeof(resp));
    assert(len > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Set-Cookie: cj_sid=node42; Path=/; HttpOnly\r\n") != NULL);
    assert(strstr(resp, "Content-Length: 14\r\n") != NULL);
    assert(strstr(resp, "session active") != NULL);

    printf("OK: cj_portal_session_v1 passed\n");
    return 0;
}
