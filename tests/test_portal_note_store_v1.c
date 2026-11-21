#include "cj_portal_note_store_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[128];
    cj_portal_note_store_set("first note");
    assert(cj_portal_note_store_get(buf, sizeof(buf)) == 10);
    assert(strcmp(buf, "first note") == 0);

    char resp[2048];
    size_t n = cj_portal_build_note_last_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"last_note\":\"first note\"}") != NULL);

    printf("OK: cj_portal_note_store_v1 passed\n");
    return 0;
}
