#ifndef CJ_NOTE_DB_V1_H
#define CJ_NOTE_DB_V1_H

#include <stddef.h>

#ifndef CJ_NOTE_DB_MAX_RECORDS
#define CJ_NOTE_DB_MAX_RECORDS 256u
#endif

#ifndef CJ_NOTE_TITLE_MAX
#define CJ_NOTE_TITLE_MAX 96u
#endif

#ifndef CJ_NOTE_BODY_MAX
#define CJ_NOTE_BODY_MAX 512u
#endif

typedef struct {
    unsigned int id;
    char title[CJ_NOTE_TITLE_MAX];
    char body[CJ_NOTE_BODY_MAX];
} cj_note_record_t;

void cj_note_db_init(void);
int cj_note_db_create(const char *title, const char *body, unsigned int *out_id);
int cj_note_db_get(unsigned int id, cj_note_record_t *out_rec);
int cj_note_db_update(unsigned int id, const char *title, const char *body);
int cj_note_db_delete(unsigned int id);
size_t cj_note_db_count(void);
int cj_note_db_save(const char *path);
int cj_note_db_load(const char *path);
size_t cj_note_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap);

#endif
