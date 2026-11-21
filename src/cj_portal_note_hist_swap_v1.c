#include "cj_portal_note_hist_swap_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_SWAP_MAX_ITEMS
#define CJ_SWAP_MAX_ITEMS 128u
#endif

int cj_portal_note_hist_swap(size_t idx_a, size_t idx_b) {
    size_t cnt = cj_portal_note_hist_count();
    if (idx_a >= cnt || idx_b >= cnt) return -1;
    if (idx_a == idx_b) return 0;

    size_t n = (cnt < CJ_SWAP_MAX_ITEMS) ? cnt : CJ_SWAP_MAX_ITEMS;
    if (idx_a >= n || idx_b >= n) return -1;

    char items[CJ_SWAP_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_get(i, items[i], sizeof(items[i]));
    }

    char tmp[CJ_NOTE_HIST_ITEM_MAX];
    memcpy(tmp, items[idx_a], sizeof(tmp));
    memcpy(items[idx_a], items[idx_b], sizeof(tmp));
    memcpy(items[idx_b], tmp, sizeof(tmp));

    cj_portal_note_hist_clear();
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_push(items[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_swap_resp(size_t idx_a, size_t idx_b, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (cj_portal_note_hist_swap(idx_a, idx_b) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"swapped\":false,\"error\":\"index_out_of_range\"}",
            out_buf,
            out_cap
        );
    }

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"swapped\":true,\"a\":%zu,\"b\":%zu}", idx_a, idx_b);
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
