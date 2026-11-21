#include "cj_http_hdr_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *req =
        "GET /index.html HTTP/1.1\r\n"
        "Host: 127.0.0.1:8080\r\n"
        "User-Agent: cjang-client/1.0\r\n"
        "\r\n";

    char host[64];
    char ua[64];
    char missing[32];

    assert(cj_http_get_header(req, "host", host, sizeof(host)) == 0);
    assert(strcmp(host, "127.0.0.1:8080") == 0);

    assert(cj_http_get_header(req, "User-Agent", ua, sizeof(ua)) == 0);
    assert(strcmp(ua, "cjang-client/1.0") == 0);

    assert(cj_http_get_header(req, "Authorization", missing, sizeof(missing)) != 0);

    printf("OK: cj_http_hdr_v1 passed\n");
    return 0;
}
