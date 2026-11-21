#include "cj_portal_note_store_v1.h"
#include "cj_http_json_v1.h"
#include <string.h>

static char g_last_note[CJ_NOTE_STORE_MAX] = "";

void cj_portal_note_store_set(const char *note) {
    if (!note) {
        g_last_note[0] = '\0';
        return;
    }
    size_t n = strlen(note);
    if (n >= sizeof(g_last_note)) n = sizeof(g_last_note) - 1;
    if (n > 0) memcpy(g_last_note, note, n);
    g_last_note[n] = '\0';
}

size_t cj_portal_note_store_get(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    size_t n = strlen(g_last_note);
    if (n >= out_cap) n = out_cap - 1;
    if (n > 0) memcpy(out_buf, g_last_note, n);
    out_buf[n] = '\0';
    return n;
}

size_t cj_portal_build_note_last_resp(char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    char cur[CJ_NOTE_STORE_MAX];
    cj_portal_note_store_get(cur, sizeof(cur));
    return cj_http_build_json_kv(200, "OK", "last_note", cur, out_buf, out_cap);
}
