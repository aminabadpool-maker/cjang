#include "cj_portal_note_hist_dup_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_note_hist_dup_count(const char *needle) {
    if (!needle || needle[0] == '\0') return 0;

    size_t cnt = cj_portal_note_hist_count();
    size_t matches = 0;

    for (size_t i = 0; i < cnt; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(i, item, sizeof(item));
        if (strcmp(item, needle) == 0) {
            matches++;
        }
    }

    return matches;
}

size_t cj_portal_build_note_hist_dup_resp(const char *needle, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    const char *q = needle ? needle : "";
    size_t matches = cj_portal_note_hist_dup_count(q);
    int exists = (matches > 0) ? 1 : 0;

    char q_esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(q, q_esc, sizeof(q_esc));

    char body[CJ_NOTE_HIST_ITEM_MAX * 2 + 128];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"query\":\"%s\",\"exists\":%s,\"matches\":%zu}",
        q_esc,
        exists ? "true" : "false",
        matches
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
