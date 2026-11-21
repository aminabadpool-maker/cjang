#include "cj_portal_note_hist_prefix_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("sys_boot");
    cj_portal_note_hist_push("net_rx");
    cj_portal_note_hist_push("sys_tick");

    assert(cj_portal_note_hist_prefix_count("sys_") == 2);
    assert(cj_portal_note_hist_prefix_count("net_") == 1);
    assert(cj_portal_note_hist_prefix_count("none_") == 0);

    char resp[2048];
    size_t n = cj_portal_build_note_hist_prefix_resp("sys_", resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"prefix\":\"sys_\",\"count\":2,\"items\":[\"sys_boot\",\"sys_tick\"]}") != NULL);

    printf("OK: cj_portal_note_hist_prefix_v1 passed\n");
    return 0;
}
