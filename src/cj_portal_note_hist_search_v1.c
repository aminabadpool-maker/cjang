#include "cj_portal_note_hist_search_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

static int item_matches(const char *item, const char *query) {
    if (!item) return 0;
    if (!query || query[0] == '\0') return 1;
    return (strstr(item, query) != NULL) ? 1 : 0;
}

size_t cj_portal_note_hist_search_count(const char *query) {
    size_t cnt = cj_portal_note_hist_count();
    size_t matched = 0;

    for (size_t i = 0; i < cnt; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(i, item, sizeof(item));
        if (item_matches(item, query)) {
            matched++;
        }
    }
    return matched;
}

size_t cj_portal_build_note_hist_search_resp(const char *query, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    const char *q = query ? query : "";
    size_t cnt = cj_portal_note_hist_count();
    size_t matched = cj_portal_note_hist_search_count(q);

    char q_esc[CJ_NOTE_HIST_ITEM_MAX * 2];
    cj_json_escape(q, q_esc, sizeof(q_esc));

    char body[2048];
    size_t pos = 0;

    int p0 = snprintf(
        body + pos,
        sizeof(body) - pos,
        "{\"query\":\"%s\",\"count\":%zu,\"items\":[",
        q_esc,
        matched
    );
    if (p0 <= 0 || (size_t)p0 >= sizeof(body) - pos) return 0;
    pos += (size_t)p0;

    size_t emitted = 0;
    for (size_t i = 0; i < cnt; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(i, item, sizeof(item));
        if (!item_matches(item, q)) continue;

        char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
        cj_json_escape(item, esc, sizeof(esc));

        int pi = snprintf(
            body + pos,
            sizeof(body) - pos,
            "%s\"%s\"",
            (emitted > 0) ? "," : "",
            esc
        );
        if (pi <= 0 || (size_t)pi >= sizeof(body) - pos) return 0;
        pos += (size_t)pi;
        emitted++;
    }

    int pend = snprintf(body + pos, sizeof(body) - pos, "]}");
    if (pend <= 0 || (size_t)pend >= sizeof(body) - pos) return 0;
    pos += (size_t)pend;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        out_buf,
        out_cap
    );
}
