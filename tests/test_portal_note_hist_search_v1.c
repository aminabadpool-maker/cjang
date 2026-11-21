#include "cj_portal_note_hist_search_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("alpha node");
    cj_portal_note_hist_push("beta queue");
    cj_portal_note_hist_push("alpha ring");

    assert(cj_portal_note_hist_search_count("alpha") == 2);
    assert(cj_portal_note_hist_search_count("queue") == 1);
    assert(cj_portal_note_hist_search_count("missing") == 0);

    char resp[2048];
    size_t n = cj_portal_build_note_hist_search_resp("alpha", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"query\":\"alpha\",\"count\":2,\"items\":[\"alpha node\",\"alpha ring\"]}") != NULL);

    printf("OK: cj_portal_note_hist_search_v1 passed\n");
    return 0;
}
