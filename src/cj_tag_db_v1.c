#include "cj_tag_db_v1.h"
#include "cj_http_json_v1.h"
#include <stdio.h>
#include <string.h>

static cj_tag_record_t g_tags[CJ_TAG_MAX_RECORDS];
static size_t g_tag_count = 0u;
static unsigned int g_next_tag_id = 1u;

static void copy_tag_bounded(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    size_t len = strlen(src);
    if (len >= cap) len = cap - 1u;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

static int find_tag_index(unsigned int id, size_t *out_idx) {
    for (size_t i = 0u; i < g_tag_count; i++) {
        if (g_tags[i].id == id) {
            if (out_idx) *out_idx = i;
            return 0;
        }
    }
    return -1;
}

void cj_tag_db_init(void) {
    memset(g_tags, 0, sizeof(g_tags));
    g_tag_count = 0u;
    g_next_tag_id = 1u;
}

int cj_tag_db_add(unsigned int note_id, const char *name, unsigned int *out_id) {
    if (!name || name[0] == '\0') return -1;
    if (g_tag_count >= CJ_TAG_MAX_RECORDS) return -2;

    cj_tag_record_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.id = g_next_tag_id++;
    rec.note_id = note_id;
    copy_tag_bounded(rec.name, sizeof(rec.name), name);

    g_tags[g_tag_count++] = rec;
    if (out_id) *out_id = rec.id;
    return 0;
}

int cj_tag_db_get(unsigned int id, cj_tag_record_t *out_rec) {
    if (!out_rec) return -1;
    size_t idx = 0u;
    if (find_tag_index(id, &idx) != 0) return -1;
    *out_rec = g_tags[idx];
    return 0;
}

size_t cj_tag_db_count(void) {
    return g_tag_count;
}

size_t cj_tag_db_count_by_note(unsigned int note_id) {
    size_t cnt = 0u;
    for (size_t i = 0u; i < g_tag_count; i++) {
        if (g_tags[i].note_id == note_id) cnt++;
    }
    return cnt;
}

int cj_tag_db_delete(unsigned int id) {
    size_t idx = 0u;
    if (find_tag_index(id, &idx) != 0) return -1;

    for (size_t i = idx; i + 1u < g_tag_count; i++) {
        g_tags[i] = g_tags[i + 1u];
    }
    memset(&g_tags[g_tag_count - 1u], 0, sizeof(cj_tag_record_t));
    g_tag_count--;
    return 0;
}

int cj_tag_db_save(const char *path) {
    if (!path || path[0] == '\0') return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;

    struct {
        char magic[8];
        unsigned int next_id;
        size_t count;
    } hdr;

    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, "CJTAG01", 7);
    hdr.next_id = g_next_tag_id;
    hdr.count = g_tag_count;

    if (fwrite(&hdr, sizeof(hdr), 1u, fp) != 1u) {
        fclose(fp);
        return -1;
    }
    if (g_tag_count > 0u && fwrite(g_tags, sizeof(cj_tag_record_t), g_tag_count, fp) != g_tag_count) {
        fclose(fp);
        return -1;
    }
    if (fclose(fp) != 0) return -1;
    return 0;
}

int cj_tag_db_load(const char *path) {
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
    if (memcmp(hdr.magic, "CJTAG01", 7) != 0 || hdr.count > CJ_TAG_MAX_RECORDS) {
        fclose(fp);
        return -1;
    }

    cj_tag_record_t tmp[CJ_TAG_MAX_RECORDS];
    memset(tmp, 0, sizeof(tmp));
    if (hdr.count > 0u && fread(tmp, sizeof(cj_tag_record_t), hdr.count, fp) != hdr.count) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    memcpy(g_tags, tmp, sizeof(tmp));
    g_tag_count = hdr.count;
    g_next_tag_id = (hdr.next_id > 0u) ? hdr.next_id : 1u;
    return 0;
}

size_t cj_tag_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0u) return 0u;
    out_buf[0] = '\0';

    cj_tag_record_t rec;
    if (cj_tag_db_get(id, &rec) != 0) return 0u;

    char name_esc[CJ_TAG_NAME_MAX * 2];
    cj_json_escape(rec.name, name_esc, sizeof(name_esc));

    int n = snprintf(
        out_buf,
        out_cap,
        "{\"id\":%u,\"note_id\":%u,\"name\":\"%s\"}",
        rec.id,
        rec.note_id,
        name_esc
    );
    if (n <= 0 || (size_t)n >= out_cap) {
        out_buf[0] = '\0';
        return 0u;
    }
    return (size_t)n;
}
