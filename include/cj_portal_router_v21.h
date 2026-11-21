#ifndef CJ_PORTAL_ROUTER_V21_H
#define CJ_PORTAL_ROUTER_V21_H

#include <stddef.h>

size_t cj_portal_route_v21(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
);

#endif
