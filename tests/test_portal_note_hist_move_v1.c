#include "cj_portal_note_hist_move_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("m0");
    cj_portal_note_hist_push("m1");
    cj_portal_note_hist_push("m2");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_move_resp(0u, 2u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"moved\":true,\"from\":0,\"to\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 2);
    assert(strcmp(item, "m1") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 2);
    assert(strcmp(item, "m2") == 0);
    assert(cj_portal_note_hist_get(2, item, sizeof(item)) == 2);
    assert(strcmp(item, "m0") == 0);

    size_t n_bad = cj_portal_build_note_hist_move_resp(0u, 8u, resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_note_hist_move_v1 passed\n");
    return 0;
}
