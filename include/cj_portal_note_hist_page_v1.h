#ifndef CJ_PORTAL_NOTE_HIST_PAGE_V1_H
#define CJ_PORTAL_NOTE_HIST_PAGE_V1_H

#include <stddef.h>

size_t cj_portal_build_note_hist_page_resp(
    size_t offset,
    size_t limit,
    char *out_buf,
    size_t out_cap
);

#endif
