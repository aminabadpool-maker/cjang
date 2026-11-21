#include "cj_http_json_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char esc[128];
    size_t elen = cj_json_escape("line1\n\"quoted\"", esc, sizeof(esc));
    assert(elen > 0);
    assert(strcmp(esc, "line1\\n\\\"quoted\\\"") == 0);

    char resp[2048];
    size_t rlen = cj_http_build_json_kv(200, "OK", "status", "ok", resp, sizeof(resp));
    assert(rlen > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Content-Type: application/json; charset=utf-8\r\n") != NULL);
    assert(strstr(resp, "{\"status\":\"ok\"}") != NULL);

    printf("OK: cj_http_json_v1 passed\n");
    return 0;
}
