#include "cj_portal_note_hist_export_v1.h"
#include "cj_portal_note_hist_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_note_hist_export_text(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t cnt = cj_portal_note_hist_count();
    size_t pos = 0;

    for (size_t i = 0; i < cnt; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        cj_portal_note_hist_get(i, item, sizeof(item));
        size_t len = strlen(item);

        if (pos + len + 1 >= out_cap) return 0;
        if (len > 0) {
            memcpy(out_buf + pos, item, len);
            pos += len;
        }
        out_buf[pos++] = '\n';
        out_buf[pos] = '\0';
    }

    return pos;
}

size_t cj_portal_build_note_hist_export_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char body[2048];
    size_t blen = cj_portal_note_hist_export_text(body, sizeof(body));
    if (blen >= sizeof(body)) return 0;

    int n = snprintf(
        out_buf,
        out_cap,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Disposition: attachment; filename=\"cjang_note_history.txt\"\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        blen,
        body
    );
    if (n <= 0 || (size_t)n >= out_cap) return 0;
    return (size_t)n;
}
