#ifndef CJ_COMMENT_DB_V1_H
#define CJ_COMMENT_DB_V1_H

#include <stddef.h>

#ifndef CJ_COMMENT_MAX_RECORDS
#define CJ_COMMENT_MAX_RECORDS 256u
#endif

#ifndef CJ_COMMENT_AUTHOR_MAX
#define CJ_COMMENT_AUTHOR_MAX 64u
#endif

#ifndef CJ_COMMENT_TEXT_MAX
#define CJ_COMMENT_TEXT_MAX 256u
#endif

typedef enum {
    CJ_COMMENT_PENDING  = 0,
    CJ_COMMENT_APPROVED = 1,
    CJ_COMMENT_HIDDEN   = 2
} cj_comment_status_t;

typedef struct {
    unsigned int id;
    unsigned int note_id;
    int status;
    char author[CJ_COMMENT_AUTHOR_MAX];
    char text[CJ_COMMENT_TEXT_MAX];
} cj_comment_record_t;

void cj_comment_db_init(void);
int cj_comment_db_add(unsigned int note_id, const char *author, const char *text, unsigned int *out_id);
int cj_comment_db_get(unsigned int id, cj_comment_record_t *out_rec);
size_t cj_comment_db_count(void);
size_t cj_comment_db_count_by_note(unsigned int note_id);
int cj_comment_db_set_status(unsigned int id, int status);
int cj_comment_db_delete(unsigned int id);
int cj_comment_db_save(const char *path);
int cj_comment_db_load(const char *path);
size_t cj_comment_db_format_item_json(unsigned int id, char *out_buf, size_t out_cap);

#endif
