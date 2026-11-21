#include "cj_portal_note_hist_trim_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("t0");
    cj_portal_note_hist_push("t1");
    cj_portal_note_hist_push("t2");
    cj_portal_note_hist_push("t3");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_trim_resp(2u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"trimmed\":2,\"count\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 2);
    assert(strcmp(item, "t2") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 2);
    assert(strcmp(item, "t3") == 0);

    printf("OK: cj_portal_note_hist_trim_v1 passed\n");
    return 0;
}
