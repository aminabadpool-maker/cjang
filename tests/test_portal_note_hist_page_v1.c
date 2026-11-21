#include "cj_portal_note_hist_page_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("n0");
    cj_portal_note_hist_push("n1");
    cj_portal_note_hist_push("n2");
    cj_portal_note_hist_push("n3");

    char resp[2048];
    size_t n = cj_portal_build_note_hist_page_resp(1u, 2u, resp, sizeof(resp));
    assert(n > 0);
    assert(strstr(resp, "{\"total\":4,\"offset\":1,\"limit\":2,\"count\":2,\"items\":[\"n1\",\"n2\"]}") != NULL);

    printf("OK: cj_portal_note_hist_page_v1 passed\n");
    return 0;
}
