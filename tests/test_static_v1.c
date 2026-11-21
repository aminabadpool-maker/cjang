#include "cj_static_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    assert(cj_static_is_safe_relpath("index.html") == 1);
    assert(cj_static_is_safe_relpath("css/style.css") == 1);
    assert(cj_static_is_safe_relpath("../etc/passwd") == 0);
    assert(cj_static_is_safe_relpath("/etc/passwd") == 0);

    char body[CJ_STATIC_BODY_MAX];
    size_t len = 0;
    const char *mime = NULL;

    int rc_html = cj_static_read_file("public", "index.html", body, sizeof(body), &len, &mime);
    assert(rc_html == 0);
    assert(len > 0);
    assert(strcmp(mime, "text/html; charset=utf-8") == 0);

    int rc_css = cj_static_read_file("public", "style.css", body, sizeof(body), &len, &mime);
    assert(rc_css == 0);
    assert(len > 0);
    assert(strcmp(mime, "text/css; charset=utf-8") == 0);

    int rc_bad = cj_static_read_file("public", "../Makefile_v3", body, sizeof(body), &len, &mime);
    assert(rc_bad != 0);

    printf("OK: cj_static_v1 passed\n");
    return 0;
}
