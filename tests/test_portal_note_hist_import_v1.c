#include "cj_portal_note_hist_import_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("old_item");

    char resp[1024];
    size_t n1 = cj_portal_build_note_hist_import_resp("imp_a\nimp_b\n", 1, resp, sizeof(resp));
    assert(n1 > 0);
    assert(strstr(resp, "{\"imported\":2,\"count\":2}") != NULL);

    char item[64];
    assert(cj_portal_note_hist_get(0, item, sizeof(item)) == 5);
    assert(strcmp(item, "imp_a") == 0);
    assert(cj_portal_note_hist_get(1, item, sizeof(item)) == 5);
    assert(strcmp(item, "imp_b") == 0);

    size_t n2 = cj_portal_build_note_hist_import_resp("imp_c\n", 0, resp, sizeof(resp));
    assert(n2 > 0);
    assert(strstr(resp, "{\"imported\":1,\"count\":3}") != NULL);

    printf("OK: cj_portal_note_hist_import_v1 passed\n");
    return 0;
}
