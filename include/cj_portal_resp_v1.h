#ifndef CJ_PORTAL_RESP_V1_H
#define CJ_PORTAL_RESP_V1_H

#include <stddef.h>

size_t cj_portal_build_http_200(
    const char *title,
    const char *heading,
    const char *body_text,
    char *resp_buf,
    size_t resp_cap
);

#endif
