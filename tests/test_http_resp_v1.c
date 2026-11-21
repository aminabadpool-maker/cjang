#include "cj_http_resp_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char resp[2048];
    const char *body = "<h1>cjang ok</h1>";

    size_t len = cj_http_build_text_resp(200, "OK", "text/html; charset=utf-8", body, resp, sizeof(resp));
    assert(len > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Content-Type: text/html; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "Content-Length: 17\r\n") != NULL);
    assert(strstr(resp, "<h1>cjang ok</h1>") != NULL);

    printf("OK: cj_http_resp_v1 passed\n");
    return 0;
}
