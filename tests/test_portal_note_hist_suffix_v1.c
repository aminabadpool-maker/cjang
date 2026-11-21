#include "cj_portal_note_hist_suffix_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("node_v1");
    cj_portal_note_hist_push("core_v2");
    cj_portal_note_hist_push("router_v1");

    assert(cj_portal_note_hist_suffix_count("_v1") == 2);
    assert(cj_portal_note_hist_suffix_count("_v2") == 1);
    assert(cj_portal_note_hist_suffix_count("_v9") == 0);

    char resp[2048];
    size_t n = cj_portal_build_note_hist_suffix_resp("_v1", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"suffix\":\"_v1\",\"count\":2,\"items\":[\"node_v1\",\"router_v1\"]}") != NULL);

    printf("OK: cj_portal_note_hist_suffix_v1 passed\n");
    return 0;
}
