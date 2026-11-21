#ifndef CJ_NOTE_THREAD_V1_H
#define CJ_NOTE_THREAD_V1_H

#include <stddef.h>

size_t cj_note_thread_count_comments(unsigned int note_id);
size_t cj_note_thread_format_json(unsigned int note_id, char *out_buf, size_t out_cap);
size_t cj_note_thread_http_resp(const char *req_buf, char *resp_buf, size_t resp_cap);

#endif
