#include "cj_portal_note_hist_trim_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_TRIM_MAX_ITEMS
#define CJ_TRIM_MAX_ITEMS 128u
#endif

size_t cj_portal_note_hist_trim(size_t keep) {
    size_t orig_cnt = cj_portal_note_hist_count();
    if (keep >= orig_cnt) return 0;

    size_t cap = (keep < CJ_TRIM_MAX_ITEMS) ? keep : CJ_TRIM_MAX_ITEMS;
    char tail_items[CJ_TRIM_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    size_t start = orig_cnt - cap;

    for (size_t i = 0; i < cap; i++) {
        char cur[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(start + i, cur, sizeof(cur));
        size_t len = strlen(cur);
        if (len >= CJ_NOTE_HIST_ITEM_MAX) len = CJ_NOTE_HIST_ITEM_MAX - 1u;
        memcpy(tail_items[i], cur, len);
        tail_items[i][len] = '\0';
    }

    cj_portal_note_hist_clear();
    for (size_t i = 0; i < cap; i++) {
        cj_portal_note_hist_push(tail_items[i]);
    }

    size_t new_cnt = cj_portal_note_hist_count();
    return (orig_cnt > new_cnt) ? (orig_cnt - new_cnt) : 0;
}

size_t cj_portal_build_note_hist_trim_resp(size_t keep, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t trimmed = cj_portal_note_hist_trim(keep);
    size_t cnt = cj_portal_note_hist_count();

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"trimmed\":%zu,\"count\":%zu}", trimmed, cnt);
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
