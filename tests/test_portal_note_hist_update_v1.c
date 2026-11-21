#include "cj_portal_note_hist_update_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("first");
    cj_portal_note_hist_push("old_mid");
    cj_portal_note_hist_push("last");

    char resp[1024];
    size_t n = cj_portal_build_note_hist_update_resp(1u, "new_mid", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"updated\":true,\"index\":1,\"text\":\"new_mid\",\"count\":3}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 7);
    assert(strcmp(item, "new_mid") == 0);

    size_t n_bad = cj_portal_build_note_hist_update_resp(8u, "nope", resp, sizeof(resp));
    assert(n_bad > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    printf("OK: cj_portal_note_hist_update_v1 passed\n");
    return 0;
}
