#ifndef CJ_COMMENT_HTTP_V1_H
#define CJ_COMMENT_HTTP_V1_H

#include <stddef.h>

size_t cj_comment_http_add_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_comment_http_list_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_comment_http_moderate_resp(const char *req_buf, char *resp_buf, size_t resp_cap);
size_t cj_comment_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap);

#endif
