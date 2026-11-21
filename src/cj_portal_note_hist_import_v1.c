#include "cj_portal_note_hist_import_v1.h"
#include "cj_portal_note_hist_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_note_hist_import_text(const char *raw_text, int clear_first) {
    if (clear_first) {
        cj_portal_note_hist_clear();
    }
    if (!raw_text || raw_text[0] == '\0') {
        return 0;
    }

    size_t imported = 0;
    const char *p = raw_text;

    while (*p != '\0') {
        const char *eol = p;
        while (*eol != '\0' && *eol != '\n' && *eol != '\r') {
            eol++;
        }

        size_t len = (size_t)(eol - p);
        if (len > 0) {
            char line[CJ_NOTE_HIST_ITEM_MAX];
            size_t copy_len = (len >= sizeof(line)) ? (sizeof(line) - 1u) : len;
            memcpy(line, p, copy_len);
            line[copy_len] = '\0';
            cj_portal_note_hist_push(line);
            imported++;
        }

        p = eol;
        while (*p == '\r' || *p == '\n') {
            p++;
        }
    }

    return imported;
}

size_t cj_portal_build_note_hist_import_resp(const char *raw_text, int clear_first, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    size_t imported = cj_portal_note_hist_import_text(raw_text, clear_first);

    char body[160];
    int n = snprintf(
        body,
        sizeof(body),
        "{\"imported\":%zu,\"count\":%zu}",
        imported,
        cj_portal_note_hist_count()
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
