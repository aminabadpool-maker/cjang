#ifndef CJ_PORTAL_HEALTH_V1_H
#define CJ_PORTAL_HEALTH_V1_H

#include <stddef.h>

size_t cj_portal_build_health_resp(
    const char *req_buf,
    const char *cookie_name,
    const char *expected_sid,
    char *out_buf,
    size_t out_cap
);

#endif
