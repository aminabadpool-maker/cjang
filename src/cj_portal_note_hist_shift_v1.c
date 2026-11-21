#include "cj_portal_note_hist_shift_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_SHIFT_MAX_ITEMS
#define CJ_SHIFT_MAX_ITEMS 128u
#endif

int cj_portal_note_hist_shift(char *out_item, size_t out_cap) {
    if (out_item && out_cap > 0) out_item[0] = '\0';

    size_t cnt = cj_portal_note_hist_count();
    if (cnt == 0) return -1;

    size_t n = (cnt < CJ_SHIFT_MAX_ITEMS) ? cnt : CJ_SHIFT_MAX_ITEMS;
    char items[CJ_SHIFT_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_get(i, items[i], sizeof(items[i]));
    }

    if (out_item && out_cap > 0) {
        size_t len = strlen(items[0]);
        if (len >= out_cap) len = out_cap - 1u;
        memcpy(out_item, items[0], len);
        out_item[len] = '\0';
    }

    cj_portal_note_hist_clear();
    for (size_t i = 1u; i < n; i++) {
        cj_portal_note_hist_push(items[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_shift_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char shifted_item[CJ_NOTE_HIST_ITEM_MAX];
    if (cj_portal_note_hist_shift(shifted_item, sizeof(shifted_item)) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"shifted\":false,\"error\":\"empty_history\"}",
            out_buf,
            out_cap
        );
    }

    size_t rem_cnt = cj_portal_note_hist_count();
    char item_esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(shifted_item, item_esc, sizeof(item_esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 2 + 128];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"shifted\":true,\"item\":\"%s\",\"count\":%zu}",
        item_esc,
        rem_cnt
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
