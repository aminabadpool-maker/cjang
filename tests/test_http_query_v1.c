#include "cj_http_query_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *req = "GET /page?node=cjang+01&msg=hi%21 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    char node[64];
    char msg[64];
    char miss[32];

    assert(cj_http_get_query_param(req, "node", node, sizeof(node)) == 0);
    assert(strcmp(node, "cjang 01") == 0);

    assert(cj_http_get_query_param(req, "msg", msg, sizeof(msg)) == 0);
    assert(strcmp(msg, "hi!") == 0);

    assert(cj_http_get_query_param(req, "other", miss, sizeof(miss)) != 0);

    printf("OK: cj_http_query_v1 passed\n");
    return 0;
}
