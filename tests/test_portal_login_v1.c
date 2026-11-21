#include "cj_portal_login_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];

    const char *ok_req = "POST /login HTTP/1.1\r\nHost: localhost\r\n\r\nuser=admin";
    size_t n1 = cj_portal_handle_login(ok_req, "cj_sid", "node42", resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Set-Cookie: cj_sid=node42; Path=/; HttpOnly\r\n") != NULL);
    assert(strstr(resp, "logged in: admin") != NULL);

    const char *bad_req = "POST /login HTTP/1.1\r\nHost: localhost\r\n\r\nuser=guest";
    size_t n2 = cj_portal_handle_login(bad_req, "cj_sid", "node42", resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "HTTP/1.1 401 Unauthorized\r\n") != NULL);
    assert(strstr(resp, "unauthorized") != NULL);

    printf("OK: cj_portal_login_v1 passed\n");
    return 0;
}
