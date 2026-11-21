#include "cj_portal_note_hist_delete_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

int cj_portal_note_hist_delete_at(size_t idx_from_oldest) {
    size_t cnt = cj_portal_note_hist_count();
    if (idx_from_oldest >= cnt || cnt > CJ_NOTE_HIST_SLOTS) {
        return -1;
    }

    char kept[CJ_NOTE_HIST_SLOTS][CJ_NOTE_HIST_ITEM_MAX];
    size_t kept_n = 0;

    for (size_t i = 0; i < cnt; i++) {
        if (i == idx_from_oldest) continue;
        cj_portal_note_hist_get(i, kept[kept_n], sizeof(kept[kept_n]));
        kept_n++;
    }

    cj_portal_note_hist_clear();
    for (size_t i = 0; i < kept_n; i++) {
        cj_portal_note_hist_push(kept[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_delete_resp(size_t idx_from_oldest, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (cj_portal_note_hist_delete_at(idx_from_oldest) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"deleted\":false,\"error\":\"index_out_of_range\"}",
            out_buf,
            out_cap
        );
    }

    char body[160];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"deleted\":true,\"index\":%zu,\"count\":%zu}",
        idx_from_oldest,
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
