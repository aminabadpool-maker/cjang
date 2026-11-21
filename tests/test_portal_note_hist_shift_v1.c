#include "cj_portal_note_hist_shift_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[1024];

    size_t n_empty = cj_portal_build_note_hist_shift_resp(resp, sizeof(resp));
    assert(n_empty > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    cj_portal_note_hist_push("s_first");
    cj_portal_note_hist_push("s_second");

    size_t n = cj_portal_build_note_hist_shift_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"shifted\":true,\"item\":\"s_first\",\"count\":1}") != NULL);
    assert(cj_portal_note_hist_count() == 1);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 8);
    assert(strcmp(item, "s_second") == 0);

    printf("OK: cj_portal_note_hist_shift_v1 passed\n");
    return 0;
}
