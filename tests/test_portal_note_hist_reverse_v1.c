#include "cj_portal_note_hist_reverse_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("first");
    cj_portal_note_hist_push("second");
    cj_portal_note_hist_push("third");

    char resp[2048];
    size_t n = cj_portal_build_note_hist_reverse_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"count\":3,\"items\":[\"third\",\"second\",\"first\"]}") != NULL);

    printf("OK: cj_portal_note_hist_reverse_v1 passed\n");
    return 0;
}
