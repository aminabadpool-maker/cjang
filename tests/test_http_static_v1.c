#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];
    size_t rlen = 0;

    assert(cj_http_build_static_200("public", "index.html", resp, sizeof(resp), &rlen) == 0);
    assert(rlen > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Content-Type: text/html; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    assert(cj_http_build_static_200("public", "../Makefile_v4", resp, sizeof(resp), &rlen) != 0);

    size_t n404 = cj_http_build_404(resp, sizeof(resp));
    assert(n404 > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_http_static_v1 passed\n");
    return 0;
}
