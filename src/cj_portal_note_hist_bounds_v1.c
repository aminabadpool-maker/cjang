#include "cj_portal_note_hist_bounds_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_bounds_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t cnt = cj_portal_note_hist_count();
    if (cnt == 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"found\":false,\"error\":\"empty_history\"}",
            out_buf,
            out_cap
        );
    }

    char first[CJ_NOTE_HIST_ITEM_MAX];
    char last[CJ_NOTE_HIST_ITEM_MAX];
    char first_esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    char last_esc[CJ_NOTE_HIST_ITEM_MAX * 2];

    cj_portal_note_hist_get(0, first, sizeof(first));
    cj_portal_note_hist_get(cnt - 1u, last, sizeof(last));
    cj_json_escape(first, first_esc, sizeof(first_esc));
    cj_json_escape(last, last_esc, sizeof(last_esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 4 + 160];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"found\":true,\"count\":%zu,\"first\":\"%s\",\"last\":\"%s\"}",
        cnt,
        first_esc,
        last_esc
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
