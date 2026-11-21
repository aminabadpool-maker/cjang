#ifndef CJ_PORTAL_NOTE_HIST_DUP_V1_H
#define CJ_PORTAL_NOTE_HIST_DUP_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_dup_count(const char *needle);
size_t cj_portal_build_note_hist_dup_resp(const char *needle, char *out_buf, size_t out_cap);

#endif
