#ifndef CJ_PORTAL_NOTE_STORE_V1_H
#define CJ_PORTAL_NOTE_STORE_V1_H

#include <stddef.h>

#ifndef CJ_NOTE_STORE_MAX
#define CJ_NOTE_STORE_MAX 512u
#endif

void cj_portal_note_store_set(const char *note);
size_t cj_portal_note_store_get(char *out_buf, size_t out_cap);
size_t cj_portal_build_note_last_resp(char *out_buf, size_t out_cap);

#endif
