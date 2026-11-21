#include "cj_portal_note_hist_dedup_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_DEDUP_MAX_ITEMS
#define CJ_DEDUP_MAX_ITEMS 128u
#endif

size_t cj_portal_note_hist_dedup(void) {
    size_t orig_cnt = cj_portal_note_hist_count();
    if (orig_cnt == 0) return 0;

    size_t cap = (orig_cnt < CJ_DEDUP_MAX_ITEMS) ? orig_cnt : CJ_DEDUP_MAX_ITEMS;
    char unique_items[CJ_DEDUP_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    size_t unique_cnt = 0;
    size_t removed = 0;

    for (size_t i = 0; i < orig_cnt; i++) {
        char cur[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(i, cur, sizeof(cur));

        int seen = 0;
        for (size_t j = 0; j < unique_cnt; j++) {
            if (strcmp(unique_items[j], cur) == 0) {
                seen = 1;
                break;
            }
        }

        if (seen) {
            removed++;
        } else if (unique_cnt < cap) {
            size_t len = strlen(cur);
            if (len >= CJ_NOTE_HIST_ITEM_MAX) len = CJ_NOTE_HIST_ITEM_MAX - 1u;
            memcpy(unique_items[unique_cnt], cur, len);
            unique_items[unique_cnt][len] = '\0';
            unique_cnt++;
        }
    }

    if (removed > 0) {
        cj_portal_note_hist_clear();
        for (size_t j = 0; j < unique_cnt; j++) {
            cj_portal_note_hist_push(unique_items[j]);
        }
    }

    return removed;
}

size_t cj_portal_build_note_hist_dedup_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t removed = cj_portal_note_hist_dedup();
    size_t cnt = cj_portal_note_hist_count();

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"removed\":%zu,\"count\":%zu}", removed, cnt);
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
