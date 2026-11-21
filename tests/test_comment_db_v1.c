#include "cj_comment_db_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *db_path = "build/cj_cmt_test.bin";
    remove(db_path);

    cj_comment_db_init();
    unsigned int id1 = 0, id2 = 0;

    assert(cj_comment_db_add(10u, "ali", "great c11 note", &id1) == 0);
    assert(cj_comment_db_add(10u, "sara", "second comment", &id2) == 0);
    assert(cj_comment_db_count() == 2);
    assert(cj_comment_db_count_by_note(10u) == 2);

    assert(cj_comment_db_set_status(id1, CJ_COMMENT_APPROVED) == 0);
    assert(cj_comment_db_save(db_path) == 0);

    cj_comment_db_init();
    assert(cj_comment_db_count() == 0);
    assert(cj_comment_db_load(db_path) == 0);
    assert(cj_comment_db_count() == 2);

    cj_comment_record_t rec;
    assert(cj_comment_db_get(id1, &rec) == 0);
    assert(rec.status == CJ_COMMENT_APPROVED);
    assert(strcmp(rec.author, "ali") == 0);

    char json[512];
    assert(cj_comment_db_format_item_json(id1, json, sizeof(json)) > 0);
    assert(strstr(json, "\"id\":1") != NULL);

    assert(cj_comment_db_delete(id2) == 0);
    assert(cj_comment_db_count() == 1);

    remove(db_path);
    printf("OK: cj_comment_db_v1 passed\n");
    return 0;
}
