#include "cj_portal_note_hist_clear_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("item_a");
    cj_portal_note_hist_push("item_b");
    assert(cj_portal_note_hist_count() == 2);

    char resp[1024];
    size_t n = cj_portal_build_note_hist_clear_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(cj_portal_note_hist_count() == 0);
    assert(strstr(resp, "{\"cleared\":true,\"count\":0}") != NULL);

    printf("OK: cj_portal_note_hist_clear_v1 passed\n");
    return 0;
}
