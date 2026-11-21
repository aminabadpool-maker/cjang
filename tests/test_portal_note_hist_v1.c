#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("n1");
    cj_portal_note_hist_push("n2");

    assert(cj_portal_note_hist_count() == 2);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 2);
    assert(strcmp(item, "n1") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 2);
    assert(strcmp(item, "n2") == 0);

    char resp[2048];
    size_t n = cj_portal_build_note_hist_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"count\":2,\"items\":[\"n1\",\"n2\"]}") != NULL);

    printf("OK: cj_portal_note_hist_v1 passed\n");
    return 0;
}
