#include "cj_portal_note_hist_swap_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("zero");
    cj_portal_note_hist_push("one");
    cj_portal_note_hist_push("two");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_swap_resp(0u, 2u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"swapped\":true,\"a\":0,\"b\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 3);
    assert(strcmp(item, "two") == 0);
    assert(cj_portal_note_hist_get(2, item, sizeof(item)) == 4);
    assert(strcmp(item, "zero") == 0);

    size_t n_bad = cj_portal_build_note_hist_swap_resp(0u, 9u, resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_note_hist_swap_v1 passed\n");
    return 0;
}
