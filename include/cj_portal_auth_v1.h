#ifndef CJ_PORTAL_AUTH_V1_H
#define CJ_PORTAL_AUTH_V1_H

#include <stddef.h>

int cj_portal_has_cookie_val(
    const char *req_buf,
    const char *cookie_name,
    const char *expected_val
);

#endif
