#include "cj_portal_note_hist_dedup_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("dup");
    cj_portal_note_hist_push("keep");
    cj_portal_note_hist_push("dup");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_dedup_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"removed\":1,\"count\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 3);
    assert(strcmp(item, "dup") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 4);
    assert(strcmp(item, "keep") == 0);

    printf("OK: cj_portal_note_hist_dedup_v1 passed\n");
    return 0;
}
