#include "cj_portal_note_hist_bounds_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    char resp[1024];

    size_t n_empty = cj_portal_build_note_hist_bounds_resp(resp, sizeof(resp));
    assert(n_empty > 0);
    assert(strstr(resp, "HTTP/1.1 404 Not Found\r\n") != NULL);

    cj_portal_note_hist_push("alpha");
    cj_portal_note_hist_push("beta");
    cj_portal_note_hist_push("gamma");

    size_t n = cj_portal_build_note_hist_bounds_resp(resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"found\":true,\"count\":3,\"first\":\"alpha\",\"last\":\"gamma\"}") != NULL);

    printf("OK: cj_portal_note_hist_bounds_v1 passed\n");
    return 0;
}
