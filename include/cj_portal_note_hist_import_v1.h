#ifndef CJ_PORTAL_NOTE_HIST_IMPORT_V1_H
#define CJ_PORTAL_NOTE_HIST_IMPORT_V1_H

#include <stddef.h>

size_t cj_portal_note_hist_import_text(const char *raw_text, int clear_first);
size_t cj_portal_build_note_hist_import_resp(const char *raw_text, int clear_first, char *out_buf, size_t out_cap);

#endif
