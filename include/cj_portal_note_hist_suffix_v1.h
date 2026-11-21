#ifndef CJ_PORTAL_NOTE_HIST_SUFFIX_V1_H
#define CJ_PORTAL_NOTE_HIST_SUFFIX_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_suffix_count(const char *suffix);
size_t cj_portal_build_note_hist_suffix_resp(const char *suffix, char *out_buf, size_t out_cap);

#endif
