#include "cj_portal_note_hist_export_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_portal_note_hist_clear();
    cj_portal_note_hist_push("line_a");
    cj_portal_note_hist_push("line_b");

    char txt[256];
    size_t tlen = cj_portal_note_hist_export_text(txt, sizeof(txt));
    assert(tlen == 14);
    assert(strcmp(txt, "line_a\nline_b\n") == 0);

    char resp[2048];
    size_t rlen = cj_portal_build_note_hist_export_resp(resp, sizeof(resp));
    assert(rlen > 0);
    assert(strstr(resp, "HTTP/1.1 200 OK\r\n") != NULL);
    assert(strstr(resp, "Content-Disposition: attachment; filename=\"cjang_note_history.txt\"\r\n") != NULL);
    assert(strstr(resp, "\r\n\r\nline_a\nline_b\n") != NULL);

    printf("OK: cj_portal_note_hist_export_v1 passed\n");
    return 0;
}
