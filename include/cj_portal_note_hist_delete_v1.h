#ifndef CJ_PORTAL_NOTE_HIST_DELETE_V1_H
#define CJ_PORTAL_NOTE_HIST_DELETE_V1_H

#include <stddef.h>

int cj_portal_note_hist_delete_at(size_t idx_from_oldest);
size_t cj_portal_build_note_hist_delete_resp(size_t idx_from_oldest, char *out_buf, size_t out_cap);

#endif
