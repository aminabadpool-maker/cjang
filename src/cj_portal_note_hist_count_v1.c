#include "cj_portal_note_hist_count_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_count_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char body[160];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"count\":%zu,\"max_slots\":%u}",
        cj_portal_note_hist_count(),
        (unsigned)CJ_NOTE_HIST_SLOTS
    );
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
