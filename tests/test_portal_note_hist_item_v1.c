#include "cj_portal_note_hist_item_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("zero");
    cj_portal_note_hist_push("one");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_item_resp(1u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"found\":true,\"index\":1,\"text\":\"one\"}") != NULL);

    size_t n_bad = cj_portal_build_note_hist_item_resp(5u, resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_note_hist_item_v1 passed\n");
    return 0;
}
