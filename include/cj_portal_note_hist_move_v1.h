#ifndef CJ_PORTAL_NOTE_HIST_MOVE_V1_H
#define CJ_PORTAL_NOTE_HIST_MOVE_V1_H

#include <stddef.h>

int cj_portal_note_hist_move(size_t idx_from, size_t idx_to);
size_t cj_portal_build_note_hist_move_resp(size_t idx_from, size_t idx_to, char *out_buf, size_t out_cap);

#endif
