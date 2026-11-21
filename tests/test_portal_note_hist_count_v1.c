#include "cj_portal_note_hist_count_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("c1");
    cj_portal_note_hist_push("c2");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_count_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"count\":2,\"max_slots\":8}") != NULL);

    printf("OK: cj_portal_note_hist_count_v1 passed\n");
    return 0;
}
