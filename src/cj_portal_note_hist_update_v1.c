#include "cj_portal_note_hist_update_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

int cj_portal_note_hist_update_at(size_t idx_from_oldest, const char *new_note) {
    size_t cnt = cj_portal_note_hist_count();
    if (idx_from_oldest >= cnt || cnt > CJ_NOTE_HIST_SLOTS || !new_note) {
        return -1;
    }

    char updated[CJ_NOTE_HIST_SLOTS][CJ_NOTE_HIST_ITEM_MAX];
    for (size_t i = 0; i < cnt; i++) {
        if (i == idx_from_oldest) {
            size_t len = strlen(new_note);
            if (len >= CJ_NOTE_HIST_ITEM_MAX) len = CJ_NOTE_HIST_ITEM_MAX - 1;
            if (len > 0) memcpy(updated[i], new_note, len);
            updated[i][len] = '\0';
        } else {
            cj_portal_note_hist_get(i, updated[i], sizeof(updated[i]));
        }
    }

    cj_portal_note_hist_clear();
    for (size_t i = 0; i < cnt; i++) {
        cj_portal_note_hist_push(updated[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_update_resp(size_t idx_from_oldest, const char *new_note, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (cj_portal_note_hist_update_at(idx_from_oldest, new_note) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"updated\":false,\"error\":\"index_out_of_range\"}",
            out_buf,
            out_cap
        );
    }

    const char *safe_note = new_note ? new_note : "";
    char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(safe_note, esc, sizeof(esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 2 + 128];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"updated\":true,\"index\":%zu,\"text\":\"%s\",\"count\":%zu}",
        idx_from_oldest,
        esc,
        cj_portal_note_hist_count()
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
