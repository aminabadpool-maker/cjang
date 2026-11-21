#include "cj_portal_note_hist_reverse_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>

size_t cj_portal_build_note_hist_reverse_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t cnt = cj_portal_note_hist_count();
    char body[2048];
    size_t pos = 0;

    int p0 = snprintf(body + pos, sizeof(body) - pos, "{\"count\":%zu,\"items\":[", cnt);
    if (p0 <= 0 || (size_t)p0 >= sizeof(body) - pos) return 0;
    pos += (size_t)p0;

    size_t emitted = 0;
    for (size_t i = cnt; i > 0; i--) {
        size_t idx = i - 1u;
        char item[CJ_NOTE_HIST_ITEM_MAX];
        char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
        cj_portal_note_hist_get(idx, item, sizeof(item));
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
