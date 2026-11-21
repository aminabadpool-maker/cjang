#include "cj_http_req_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char method[CJ_HTTP_METHOD_MAX];
    char target[CJ_HTTP_TARGET_MAX];

    const char *req1 = "GET /style.css?v=1 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    assert(cj_http_parse_req_line(req1, method, sizeof(method), target, sizeof(target)) == 0);
    assert(strcmp(method, "GET") == 0);
    assert(strcmp(target, "/style.css") == 0);

    const char *req2 = "GET / HTTP/1.1\r\n";
    assert(cj_http_parse_req_line(req2, method, sizeof(method), target, sizeof(target)) == 0);
    assert(strcmp(target, "/") == 0);

    assert(cj_http_parse_req_line("BADREQUEST", method, sizeof(method), target, sizeof(target)) != 0);

    printf("OK: cj_http_req_v1 passed\n");
    return 0;
}
