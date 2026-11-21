#include "cj_portal_echo_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];

    const char *q_req = "GET /echo?msg=cjang+v21 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    size_t n1 = cj_portal_handle_echo(q_req, resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "{\"echo\":\"cjang v21\"}") != NULL);

    const char *p_req = "POST /echo HTTP/1.1\r\nHost: localhost\r\n\r\nmsg=post%21";
    size_t n2 = cj_portal_handle_echo(p_req, resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "{\"echo\":\"post!\"}") != NULL);

    printf("OK: cj_portal_echo_v1 passed\n");
    return 0;
}
