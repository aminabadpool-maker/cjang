#ifndef CJ_PORTAL_NOTE_HIST_V1_H
#define CJ_PORTAL_NOTE_HIST_V1_H

#include <stddef.h>

#ifndef CJ_NOTE_HIST_SLOTS
#define CJ_NOTE_HIST_SLOTS 8u
#endif

#ifndef CJ_NOTE_HIST_ITEM_MAX
#define CJ_NOTE_HIST_ITEM_MAX 128u
#endif

void cj_portal_note_hist_clear(void);
void cj_portal_note_hist_push(const char *note);
size_t cj_portal_note_hist_count(void);
size_t cj_portal_note_hist_get(size_t idx_from_oldest, char *out_buf, size_t out_cap);
size_t cj_portal_build_note_hist_resp(char *out_buf, size_t out_cap);

#endif
