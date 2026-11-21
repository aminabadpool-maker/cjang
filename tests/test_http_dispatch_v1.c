#include "cj_http_dispatch_v1.h"
#include "cj_http_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[CJ_HTTP_RESP_MAX];

    size_t n_root = cj_http_dispatch_static("public", "GET / HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_root > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "<title>cjang portal</title>") != NULL);

    size_t n_css = cj_http_dispatch_static("public", "GET /style.css HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_css > 0);
    assert(strstr(resp, "Content-Type: text/css; charset=utf-8\r\n") != NULL);

    size_t n_trav = cj_http_dispatch_static("public", "GET /../Makefile_v5 HTTP/1.1\r\n\r\n", resp, sizeof(resp));
    assert(n_trav > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_http_dispatch_v1 passed\n");
    return 0;
}
