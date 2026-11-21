#ifndef CJ_PORTAL_NOTE_HIST_DEDUP_V1_H
#define CJ_PORTAL_NOTE_HIST_DEDUP_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_dedup(void);
size_t cj_portal_build_note_hist_dedup_resp(char *out_buf, size_t out_cap);

#endif
