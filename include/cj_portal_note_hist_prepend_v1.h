#ifndef CJ_PORTAL_NOTE_HIST_PREPEND_V1_H
#define CJ_PORTAL_NOTE_HIST_PREPEND_V1_H

#include <stddef.h>

int cj_portal_note_hist_prepend(const char *item);
size_t cj_portal_build_note_hist_prepend_resp(const char *item, char *out_buf, size_t out_cap);

#endif
