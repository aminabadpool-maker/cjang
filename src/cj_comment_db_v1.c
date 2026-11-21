#include "cj_comment_db_v1.h"
#include "cj_http_json_v1.h"
#include <stdio.h>
#include <string.h>

static cj_comment_record_t g_comments[CJ_COMMENT_MAX_RECORDS];
static size_t g_comment_count = 0;
static unsigned int g_next_id = 1u;

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

static int find_index_by_id(unsigned int id, size_t *out_idx) {
    for (size_t i = 0; i < g_comment_count; i++) {
        if (g_comments[i].id == id) {
            if (out_idx) *out_idx = i;
            return 0;
        }
    }
    return -1;
}

void cj_comment_db_init(void) {
    memset(g_comments, 0, sizeof(g_comments));
    g_comment_count = 0;
    g_next_id = 1u;
}

int cj_comment_db_add(unsigned int note_id, const char *author, const char *text, unsigned int *out_id) {
    if (!text || text[0] == '\0') return -1;
    if (g_comment_count >= CJ_COMMENT_MAX_RECORDS) return -2;

    cj_comment_record_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.id = g_next_id++;
    rec.note_id = note_id;
    rec.status = CJ_COMMENT_PENDING;
    copy_bounded(rec.author, sizeof(rec.author), (author && author[0] != '\0') ? author : "guest");
    copy_bounded(rec.text, sizeof(rec.text), text);

    g_comments[g_comment_count++] = rec;
    if (out_id) *out_id = rec.id;
    return 0;
}

int cj_comment_db_get(unsigned int id, cj_comment_record_t *out_rec) {
    if (!out_rec) return -1;
    size_t idx = 0;
    if (find_index_by_id(id, &idx) != 0) return -1;
    *out_rec = g_comments[idx];
    return 0;
}

size_t cj_comment_db_count(void) {
    return g_comment_count;
}

size_t cj_comment_db_count_by_note(unsigned int note_id) {
    size_t c = 0;
    for (size_t i = 0; i < g_comment_count; i++) {
        if (g_comments[i].note_id == note_id) c++;
    }
    return c;
}

int cj_comment_db_set_status(unsigned int id, int status) {
    if (status < CJ_COMMENT_PENDING || status > CJ_COMMENT_HIDDEN) return -1;
    size_t idx = 0;
    if (find_index_by_id(id, &idx) != 0) return -1;
    g_comments[idx].status = status;
    return 0;
}

int cj_comment_db_delete(unsigned int id) {
    size_t idx = 0;
    if (find_index_by_id(id, &idx) != 0) return -1;

    for (size_t i = idx; i + 1u < g_comment_count; i++) {
        g_comments[i] = g_comments[i + 1u];
    }
    memset(&g_comments[g_comment_count - 1u], 0, sizeof(cj_comment_record_t));
    g_comment_count--;
    return 0;
}

int cj_comment_db_save(const char *path) {
    if (!path || path[0] == '\0') return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;

    struct {
        char magic[8];
        unsigned int next_id;
        size_t count;
    } hdr;

    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, "CJCMT01", 7);
    hdr.next_id = g_next_id;
    hdr.count = g_comment_count;

    if (fwrite(&hdr, sizeof(hdr), 1u, fp) != 1u) {
        fclose(fp);
        return -1;
    }
    if (g_comment_count > 0 && fwrite(g_comments, sizeof(cj_comment_record_t), g_comment_count, fp) != g_comment_count) {
        fclose(fp);
        return -1;
    }
    if (fclose(fp) != 0) return -1;
    return 0;
}

int cj_comment_db_load(const char *path) {
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
    if (memcmp(hdr.magic, "CJCMT01", 7) != 0 || hdr.count > CJ_COMMENT_MAX_RECORDS) {
        fclose(fp);
        return -1;
    }

    cj_comment_record_t tmp[CJ_COMMENT_MAX_RECORDS];
    memset(tmp, 0, sizeof(tmp));
    if (hdr.count > 0 && fread(tmp, sizeof(cj_comment_record_t), hdr.count, fp) != hdr.count) {
        fclose(fp);
        return -1;
    }
    fclose(fp);

    memcpy(g_comments, tmp, sizeof(tmp));
    g_comment_count = hdr.count;
    g_next_id = (hdr.next_id > 0) ? hdr.next_id : 1u;
    return 0;
}

size_t cj_comment_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    cj_comment_record_t rec;
    if (cj_comment_db_get(id, &rec) != 0) return 0;

    char author_esc[CJ_COMMENT_AUTHOR_MAX * 2];
    char text_esc[CJ_COMMENT_TEXT_MAX * 2];
    cj_json_escape(rec.author, author_esc, sizeof(author_esc));
    cj_json_escape(rec.text, text_esc, sizeof(text_esc));

    int n = snprintf(
        out_buf,
        out_cap,
        "{\"id\":%u,\"note_id\":%u,\"status\":%d,\"author\":\"%s\",\"text\":\"%s\"}",
        rec.id,
        rec.note_id,
        rec.status,
        author_esc,
        text_esc
    );
    if (n <= 0 || (size_t)n >= out_cap) {
        out_buf[0] = '\0';
        return 0;
    }
    return (size_t)n;
}
