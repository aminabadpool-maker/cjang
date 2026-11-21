#include "cj_portal_note_hist_dup_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("dup_item");
    cj_portal_note_hist_push("other");
    cj_portal_note_hist_push("dup_item");

    assert(cj_portal_note_hist_dup_count("dup_item") == 2);
    assert(cj_portal_note_hist_dup_count("missing") == 0);

    char resp[1024];
    size_t n = cj_portal_build_note_hist_dup_resp("dup_item", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"query\":\"dup_item\",\"exists\":true,\"matches\":2}") != NULL);

    printf("OK: cj_portal_note_hist_dup_v1 passed\n");
    return 0;
}
