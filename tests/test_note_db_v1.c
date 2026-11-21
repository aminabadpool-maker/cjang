#include "cj_note_db_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *db_path = "build/cj_note_test.bin";
    remove(db_path);

    cj_note_db_init();
    unsigned int id1 = 0, id2 = 0;

    assert(cj_note_db_create("First Note", "Hello persistent C11", &id1) == 0);
    assert(cj_note_db_create("Second Note", "Comments ready", &id2) == 0);
    assert(cj_note_db_count() == 2);

    assert(cj_note_db_update(id1, "Updated Note", "Edited body") == 0);
    assert(cj_note_db_save(db_path) == 0);

    cj_note_db_init();
    assert(cj_note_db_count() == 0);
    assert(cj_note_db_load(db_path) == 0);
    assert(cj_note_db_count() == 2);

    cj_note_record_t rec;
    assert(cj_note_db_get(id1, &rec) == 0);
    assert(strcmp(rec.title, "Updated Note") == 0);
    assert(strcmp(rec.body, "Edited body") == 0);

    char json[1024];
    assert(cj_note_db_format_item_json(id1, json, sizeof(json)) > 0);
    assert(strstr(json, "\"id\":1") != NULL);

    assert(cj_note_db_delete(id2) == 0);
    assert(cj_note_db_count() == 1);

    remove(db_path);
    printf("OK: cj_note_db_v1 passed\n");
    return 0;
}
