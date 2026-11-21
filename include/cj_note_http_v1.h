#ifndef CJ_NOTE_HTTP_V1_H
#define CJ_NOTE_HTTP_V1_H

#include <stddef.h>

size_t cj_note_http_create_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_note_http_list_resp(char *resp_buf, size_t resp_cap);
size_t cj_note_http_update_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_note_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap);

#endif
