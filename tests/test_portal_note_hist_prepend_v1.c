#include "cj_portal_note_hist_prepend_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("second");
    cj_portal_note_hist_push("third");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_prepend_resp("first", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"prepended\":true,\"item\":\"first\",\"count\":3}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 5);
    assert(strcmp(item, "first") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 6);
    assert(strcmp(item, "second") == 0);

    size_t n_bad = cj_portal_build_note_hist_prepend_resp("", resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 400 Bad Request\r\n") != NULL);

    printf("OK: cj_portal_note_hist_prepend_v1 passed\n");
    return 0;
}
