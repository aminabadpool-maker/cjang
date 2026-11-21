#include "cj_portal_note_hist_delete_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("first");
    cj_portal_note_hist_push("middle");
    cj_portal_note_hist_push("last");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_delete_resp(1u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"deleted\":true,\"index\":1,\"count\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 5);
    assert(strcmp(item, "first") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 4);
    assert(strcmp(item, "last") == 0);

    size_t n_bad = cj_portal_build_note_hist_delete_resp(9u, resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_note_hist_delete_v1 passed\n");
    return 0;
}
