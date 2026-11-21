#include "cj_note_db_v1.h"
#include "cj_http_json_v1.h"
#include <stdio.h>
#include <string.h>

static cj_note_record_t g_notes[CJ_NOTE_DB_MAX_RECORDS];
static size_t g_note_count = 0;
static unsigned int g_next_note_id = 1u;

static void copy_bounded(char *dst, size_t dst_cap, const char *src) {
    if (!dst || dst_cap == 0) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    size_t len = strlen(src);
    if (len >= dst_cap) len = dst_cap - 1u;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

static int find_note_index(unsigned int id, size_t *out_idx) {
    for (size_t i = 0; i < g_note_count; i++) {
        if (g_notes[i].id == id) {
            if (out_idx) *out_idx = i;
            return 0;
        }
    }
    return -1;
}

void cj_note_db_init(void) {
    memset(g_notes, 0, sizeof(g_notes));
    g_note_count = 0;
    g_next_note_id = 1u;
}

int cj_note_db_create(const char *title, const char *body, unsigned int *out_id) {
    if ((!title || title[0] == '\0') && (!body || body[0] == '\0')) return -1;
    if (g_note_count >= CJ_NOTE_DB_MAX_RECORDS) return -2;

    cj_note_record_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.id = g_next_note_id++;
    copy_bounded(rec.title, sizeof(rec.title), (title && title[0] != '\0') ? title : "untitled");
    copy_bounded(rec.body, sizeof(rec.body), body ? body : "");

    g_notes[g_note_count++] = rec;
    if (out_id) *out_id = rec.id;
    return 0;
}

int cj_note_db_get(unsigned int id, cj_note_record_t *out_rec) {
    if (!out_rec) return -1;
    size_t idx = 0;
    if (find_note_index(id, &idx) != 0) return -1;
    *out_rec = g_notes[idx];
    return 0;
}

int cj_note_db_update(unsigned int id, const char *title, const char *body) {
    size_t idx = 0;
    if (find_note_index(id, &idx) != 0) return -1;
    if ((!title || title[0] == '\0') && (!body || body[0] == '\0')) return -2;

    if (title && title[0] != '\0') {
        copy_bounded(g_notes[idx].title, sizeof(g_notes[idx].title), title);
    }
    if (body && body[0] != '\0') {
        copy_bounded(g_notes[idx].body, sizeof(g_notes[idx].body), body);
    }
    return 0;
}

int cj_note_db_delete(unsigned int id) {
    size_t idx = 0;
    if (find_note_index(id, &idx) != 0) return -1;

    for (size_t i = idx; i + 1u < g_note_count; i++) {
        g_notes[i] = g_notes[i + 1u];
    }
    memset(&g_notes[g_note_count - 1u], 0, sizeof(cj_note_record_t));
    g_note_count--;
    return 0;
}

size_t cj_note_db_count(void) {
    return g_note_count;
}

int cj_note_db_save(const char *path) {
    if (!path || path[0] == '\0') return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;

    struct {
        char magic[8];
        unsigned int next_id;
        size_t count;
    } hdr;

    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, "CJNTE01", 7);
    hdr.next_id = g_next_note_id;
    hdr.count = g_note_count;

    if (fwrite(&hdr, sizeof(hdr), 1u, fp) != 1u) {
        fclose(fp);
        return -1;
    }
    if (g_note_count > 0 && fwrite(g_notes, sizeof(cj_note_record_t), g_note_count, fp) != g_note_count) {
        fclose(fp);
        return -1;
    }
    if (fclose(fp) != 0) return -1;
    return 0;
}

int cj_note_db_load(const char *path) {
    if (!path || path[0] == '\0') return -1;
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;

    struct {
        char magic[8];
        unsigned int next_id;
        size_t count;
    } hdr;

    if (fread(&hdr, sizeof(hdr), 1u, fp) != 1u) {
        fclose(fp);
        return -1;
    }
    if (memcmp(hdr.magic, "CJNTE01", 7) != 0 || hdr.count > CJ_NOTE_DB_MAX_RECORDS) {
        fclose(fp);
        return -1;
    }

    cj_note_record_t tmp[CJ_NOTE_DB_MAX_RECORDS];
    memset(tmp, 0, sizeof(tmp));
    if (hdr.count > 0 && fread(tmp, sizeof(cj_note_record_t), hdr.count, fp) != hdr.count) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    memcpy(g_notes, tmp, sizeof(tmp));
    g_note_count = hdr.count;
    g_next_note_id = (hdr.next_id > 0) ? hdr.next_id : 1u;
    return 0;
}

size_t cj_note_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    cj_note_record_t rec;
    if (cj_note_db_get(id, &rec) != 0) return 0;

    char title_esc[CJ_NOTE_TITLE_MAX * 2];
    char body_esc[CJ_NOTE_BODY_MAX * 2];
    cj_json_escape(rec.title, title_esc, sizeof(title_esc));
    cj_json_escape(rec.body, body_esc, sizeof(body_esc));

    int n = snprintf(
        out_buf,
        out_cap,
        "{\"id\":%u,\"title\":\"%s\",\"body\":\"%s\"}",
        rec.id,
        title_esc,
        body_esc
    );
    if (n <= 0 || (size_t)n >= out_cap) {
        out_buf[0] = '\0';
        return 0;
    }
    return (size_t)n;
}
