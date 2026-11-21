#include "cj_http_status_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[1024];

    size_t n400 = cj_http_build_400(resp, sizeof(resp));
    assert(n400 > 0);
    assert(strstr(resp, "HTTP/1.1 400 Bad Request\r\n") != NULL);
    assert(strstr(resp, "Bad Request") != NULL);

    size_t n405 = cj_http_build_405(resp, sizeof(resp));
    assert(n405 > 0);
    assert(strstr(resp, "HTTP/1.1 405 Method Not Allowed\r\n") != NULL);
    assert(strstr(resp, "Method Not Allowed") != NULL);

    printf("OK: cj_http_status_v1 passed\n");
    return 0;
}
