#ifndef CJ_PORTAL_NOTE_HIST_TRIM_V1_H
#define CJ_PORTAL_NOTE_HIST_TRIM_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_trim(size_t keep);
size_t cj_portal_build_note_hist_trim_resp(size_t keep, char *out_buf, size_t out_cap);

#endif
