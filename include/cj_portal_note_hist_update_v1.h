#ifndef CJ_PORTAL_NOTE_HIST_UPDATE_V1_H
#define CJ_PORTAL_NOTE_HIST_UPDATE_V1_H

#include <stddef.h>

int cj_portal_note_hist_update_at(size_t idx_from_oldest, const char *new_note);
size_t cj_portal_build_note_hist_update_resp(size_t idx_from_oldest, const char *new_note, char *out_buf, size_t out_cap);

#endif
