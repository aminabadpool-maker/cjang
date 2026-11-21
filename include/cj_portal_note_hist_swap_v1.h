#ifndef CJ_PORTAL_NOTE_HIST_SWAP_V1_H
#define CJ_PORTAL_NOTE_HIST_SWAP_V1_H

#include <stddef.h>

int cj_portal_note_hist_swap(size_t idx_a, size_t idx_b);
size_t cj_portal_build_note_hist_swap_resp(size_t idx_a, size_t idx_b, char *out_buf, size_t out_cap);

#endif
