#ifndef CJ_PORTAL_NOTE_HIST_SHIFT_V1_H
#define CJ_PORTAL_NOTE_HIST_SHIFT_V1_H

#include <stddef.h>

int cj_portal_note_hist_shift(char *out_item, size_t out_cap);
size_t cj_portal_build_note_hist_shift_resp(char *out_buf, size_t out_cap);

#endif
