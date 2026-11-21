#ifndef CJ_SEARCH_V1_H
#define CJ_SEARCH_V1_H

#include <stddef.h>

size_t cj_search_notes_json(const char *query, char *out_buf, size_t out_cap, size_t *out_count);
size_t cj_search_comments_json(const char *query, char *out_buf, size_t out_cap, size_t *out_count);
size_t cj_search_all_json(const char *query, char *out_buf, size_t out_cap);
size_t cj_search_http_resp(const char *req_buf, char *resp_buf, size_t resp_cap);

#endif
