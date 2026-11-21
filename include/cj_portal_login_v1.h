#ifndef CJ_PORTAL_LOGIN_V1_H
#define CJ_PORTAL_LOGIN_V1_H

#include <stddef.h>

size_t cj_portal_handle_login(
    const char *req_buf,
    const char *cookie_name,
    const char *session_val,
    char *out_buf,
    size_t out_cap
);

#endif
