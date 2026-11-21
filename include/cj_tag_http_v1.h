#ifndef CJ_TAG_HTTP_V1_H
#define CJ_TAG_HTTP_V1_H

#include <stddef.h>

size_t cj_tag_http_add_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_tag_http_list_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_tag_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap);

#endif
