#include "cj_portal_note_hist_page_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_page_resp(
    size_t offset,
    size_t limit,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t total = cj_portal_note_hist_count();
    size_t start = (offset < total) ? offset : total;
    size_t avail = total - start;
    size_t page_cnt = (limit < avail) ? limit : avail;

    char body[2048];
    size_t pos = 0;

    int p0 = snprintf(
        body + pos,
        sizeof(body) - pos,
        "{\"total\":%zu,\"offset\":%zu,\"limit\":%zu,\"count\":%zu,\"items\":[",
        total,
        offset,
        limit,
        page_cnt
    );
    if (p0 <= 0 || (size_t)p0 >= sizeof(body) - pos) return 0;
    pos += (size_t)p0;

    for (size_t i = 0; i < page_cnt; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
        cj_portal_note_hist_get(start + i, item, sizeof(item));
        cj_json_escape(item, esc, sizeof(esc));

        int pi = snprintf(
            body + pos,
            sizeof(body) - pos,
            "%s\"%s\"",
            (i > 0) ? "," : "",
            esc
        );
        if (pi <= 0 || (size_t)pi >= sizeof(body) - pos) return 0;
        pos += (size_t)pi;
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
