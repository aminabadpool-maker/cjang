#ifndef CJ_PORTAL_ECHO_V1_H
#define CJ_PORTAL_ECHO_V1_H

#include <stddef.h>

size_t cj_portal_handle_echo(
    const char *req_buf,
    char *out_buf,
    size_t out_cap
);

#endif
