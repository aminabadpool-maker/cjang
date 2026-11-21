#ifndef CJ_PORTAL_NOTE_HIST_PREFIX_V1_H
#define CJ_PORTAL_NOTE_HIST_PREFIX_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_prefix_count(const char *prefix);
size_t cj_portal_build_note_hist_prefix_resp(const char *prefix, char *out_buf, size_t out_cap);

#endif
