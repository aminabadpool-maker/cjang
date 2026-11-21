#include "cj_portal_note_hist_move_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_MOVE_MAX_ITEMS
#define CJ_MOVE_MAX_ITEMS 128u
#endif

int cj_portal_note_hist_move(size_t idx_from, size_t idx_to) {
    size_t cnt = cj_portal_note_hist_count();
    if (idx_from >= cnt || idx_to >= cnt) return -1;
    if (idx_from == idx_to) return 0;

    size_t n = (cnt < CJ_MOVE_MAX_ITEMS) ? cnt : CJ_MOVE_MAX_ITEMS;
    if (idx_from >= n || idx_to >= n) return -1;

    char items[CJ_MOVE_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_get(i, items[i], sizeof(items[i]));
    }

    char moving[CJ_NOTE_HIST_ITEM_MAX];
    memcpy(moving, items[idx_from], sizeof(moving));

    if (idx_from < idx_to) {
        for (size_t i = idx_from; i < idx_to; i++) {
            memcpy(items[i], items[i + 1u], sizeof(items[i]));
        }
    } else {
        for (size_t i = idx_from; i > idx_to; i--) {
            memcpy(items[i], items[i - 1u], sizeof(items[i]));
        }
    }
    memcpy(items[idx_to], moving, sizeof(items[idx_to]));

    cj_portal_note_hist_clear();
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_push(items[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_move_resp(size_t idx_from, size_t idx_to, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (cj_portal_note_hist_move(idx_from, idx_to) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"moved\":false,\"error\":\"index_out_of_range\"}",
            out_buf,
            out_cap
        );
    }

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"moved\":true,\"from\":%zu,\"to\":%zu}", idx_from, idx_to);
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
