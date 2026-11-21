#include "cj_portal_note_hist_prepend_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

#ifndef CJ_PREPEND_MAX_ITEMS
#define CJ_PREPEND_MAX_ITEMS 128u
#endif

int cj_portal_note_hist_prepend(const char *item) {
    if (!item || item[0] == '\0') return -1;

    size_t cnt = cj_portal_note_hist_count();
    size_t n = (cnt < CJ_PREPEND_MAX_ITEMS) ? cnt : CJ_PREPEND_MAX_ITEMS;

    char items[CJ_PREPEND_MAX_ITEMS][CJ_NOTE_HIST_ITEM_MAX];
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_get(i, items[i], sizeof(items[i]));
    }

    char head[CJ_NOTE_HIST_ITEM_MAX];
    size_t hlen = strlen(item);
    if (hlen >= CJ_NOTE_HIST_ITEM_MAX) hlen = CJ_NOTE_HIST_ITEM_MAX - 1u;
    memcpy(head, item, hlen);
    head[hlen] = '\0';

    cj_portal_note_hist_clear();
    cj_portal_note_hist_push(head);
    for (size_t i = 0; i < n; i++) {
        cj_portal_note_hist_push(items[i]);
    }

    return 0;
}

size_t cj_portal_build_note_hist_prepend_resp(const char *item, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    if (cj_portal_note_hist_prepend(item) != 0) {
        return cj_http_build_text_resp(
            400,
            "Bad Request",
            "application/json; charset=utf-8",
            "{\"prepended\":false,\"error\":\"empty_item\"}",
            out_buf,
            out_cap
        );
    }

    size_t cnt = cj_portal_note_hist_count();
    char item_esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(item, item_esc, sizeof(item_esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 2 + 128];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"prepended\":true,\"item\":\"%s\",\"count\":%zu}",
        item_esc,
        cnt
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
