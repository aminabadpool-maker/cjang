#include "cj_portal_note_hist_clear_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_clear_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    cj_portal_note_hist_clear();

    char body[128];
    int n = snprintf(body, sizeof(body), "{\"cleared\":true,\"count\":%zu}", cj_portal_note_hist_count());
    if (n <= 0 || (size_t)n >= sizeof(body)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        out_buf,
        out_cap
    );
}
