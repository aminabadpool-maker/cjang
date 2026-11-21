#include "cj_portal_auth_v1.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    const char *req_ok = "GET /session HTTP/1.1\r\nCookie: cj_sid=node42\r\n\r\n";
    const char *req_bad = "GET /session HTTP/1.1\r\nCookie: cj_sid=other\r\n\r\n";
    const char *req_none = "GET /session HTTP/1.1\r\n\r\n";

    assert(cj_portal_has_cookie_val(req_ok, "cj_sid", "node42") == 1);
    assert(cj_portal_has_cookie_val(req_bad, "cj_sid", "node42") == 0);
    assert(cj_portal_has_cookie_val(req_none, "cj_sid", "node42") == 0);

    printf("OK: cj_portal_auth_v1 passed\n");
    return 0;
}
