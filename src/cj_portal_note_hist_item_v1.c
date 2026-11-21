#include "cj_portal_note_hist_item_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_item_resp(size_t idx_from_oldest, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (idx_from_oldest >= cj_portal_note_hist_count()) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"found\":false,\"error\":\"index_out_of_range\"}",
            out_buf,
            out_cap
        );
    }

    char item[CJ_NOTE_HIST_ITEM_MAX];
    cj_portal_note_hist_get(idx_from_oldest, item, sizeof(item));

    char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(item, esc, sizeof(esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 2 + 128];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"found\":true,\"index\":%zu,\"text\":\"%s\"}",
        idx_from_oldest,
        esc
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
