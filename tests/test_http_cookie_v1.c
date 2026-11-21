#include "cj_http_cookie_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *req =
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Cookie: theme=dark; cj_sid=node42; lang=en\r\n"
        "\r\n";

    char sid[64];
    char theme[64];
    char miss[32];

    assert(cj_http_get_cookie(req, "cj_sid", sid, sizeof(sid)) == 0);
    assert(strcmp(sid, "node42") == 0);

    assert(cj_http_get_cookie(req, "theme", theme, sizeof(theme)) == 0);
    assert(strcmp(theme, "dark") == 0);

    assert(cj_http_get_cookie(req, "missing", miss, sizeof(miss)) != 0);

    printf("OK: cj_http_cookie_v1 passed\n");
    return 0;
}
