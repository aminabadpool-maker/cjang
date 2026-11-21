#ifndef CJ_PORTAL_SESSION_V1_H
#define CJ_PORTAL_SESSION_V1_H

#include <stddef.h>

size_t cj_portal_build_session_resp(
    const char *cookie_name,
    const char *cookie_val,
    const char *body,
    char *out_buf,
    size_t out_cap
);

#endif
