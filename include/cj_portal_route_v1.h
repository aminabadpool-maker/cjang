#ifndef CJ_PORTAL_ROUTE_V1_H
#define CJ_PORTAL_ROUTE_V1_H

#include <stddef.h>

size_t cj_portal_handle_request(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
);

#endif
