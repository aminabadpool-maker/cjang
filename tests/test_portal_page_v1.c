#include "cj_portal_page_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char html[CJ_PORTAL_PAGE_MAX];
    size_t len = cj_portal_render_html(
        "Portal Node",
        "Active Node",
        "Pure C11 static and dynamic page ready.",
        html,
        sizeof(html)
    );

    assert(len > 0);
    assert(strstr(html, "<title>Portal Node</title>") != NULL);
    assert(strstr(html, "<h1>Active Node</h1>") != NULL);
    assert(strstr(html, "<p>Pure C11 static and dynamic page ready.</p>") != NULL);

    printf("OK: cj_portal_page_v1 passed\n");
    return 0;
}
