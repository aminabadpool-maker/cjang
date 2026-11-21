#ifndef CJ_TAG_DB_V1_H
#define CJ_TAG_DB_V1_H

#include <stddef.h>

#ifndef CJ_TAG_MAX_RECORDS
#define CJ_TAG_MAX_RECORDS 256u
#endif

#ifndef CJ_TAG_NAME_MAX
#define CJ_TAG_NAME_MAX 48u
#endif

typedef struct {
    unsigned int id;
    unsigned int note_id;
    char name[CJ_TAG_NAME_MAX];
} cj_tag_record_t;

void cj_tag_db_init(void);
int cj_tag_db_add(unsigned int note_id, const char *name, unsigned int *out_id);
int cj_tag_db_get(unsigned int id, cj_tag_record_t *out_rec);
size_t cj_tag_db_count(void);
size_t cj_tag_db_count_by_note(unsigned int note_id);
int cj_tag_db_delete(unsigned int id);
int cj_tag_db_save(const char *path);
int cj_tag_db_load(const char *path);
size_t cj_tag_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap);

#endif
