#include "cj_portal_note_hist_v1.h"
#include "cj_http_json_v1.h"
#include "cj_http_resp_v1.h"
#include <stdio.h>
#include <string.h>

static char g_hist[CJ_NOTE_HIST_SLOTS][CJ_NOTE_HIST_ITEM_MAX];
static size_t g_count = 0;
static size_t g_head = 0;

void cj_portal_note_hist_clear(void) {
    memset(g_hist, 0, sizeof(g_hist));
    g_count = 0;
    g_head = 0;
}

void cj_portal_note_hist_push(const char *note) {
    const char *src = note ? note : "";
    size_t len = strlen(src);
    if (len >= CJ_NOTE_HIST_ITEM_MAX) len = CJ_NOTE_HIST_ITEM_MAX - 1;

    if (len > 0) memcpy(g_hist[g_head], src, len);
    g_hist[g_head][len] = '\0';

    g_head = (g_head + 1u) % CJ_NOTE_HIST_SLOTS;
    if (g_count < CJ_NOTE_HIST_SLOTS) g_count++;
}

size_t cj_portal_note_hist_count(void) {
    return g_count;
}

size_t cj_portal_note_hist_get(size_t idx_from_oldest, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';
    if (idx_from_oldest >= g_count) return 0;

    size_t start = (g_head + CJ_NOTE_HIST_SLOTS - g_count) % CJ_NOTE_HIST_SLOTS;
    size_t slot = (start + idx_from_oldest) % CJ_NOTE_HIST_SLOTS;
    size_t len = strlen(g_hist[slot]);
    if (len >= out_cap) len = out_cap - 1;
    if (len > 0) memcpy(out_buf, g_hist[slot], len);
    out_buf[len] = '\0';
    return len;
}

size_t cj_portal_build_note_hist_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    char body[2048];
    size_t pos = 0;

    int p0 = snprintf(body + pos, sizeof(body) - pos, "{\"count\":%zu,\"items\":[", g_count);
    if (p0 <= 0 || (size_t)p0 >= sizeof(body) - pos) return 0;
    pos += (size_t)p0;

    for (size_t i = 0; i < g_count; i++) {
        char item[CJ_NOTE_HIST_ITEM_MAX];
        char esc[CJ_NOTE_HIST_ITEM_MAX * 2];
        cj_portal_note_hist_get(i, item, sizeof(item));
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
