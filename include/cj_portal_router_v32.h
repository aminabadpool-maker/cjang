#ifndef CJ_PORTAL_ROUTER_V32_H
#define CJ_PORTAL_ROUTER_V32_H

#include <stddef.h>

size_t cj_portal_route_v32(
    const char *root_dir,
    const char *req_buf,
    char *resp_buf,
    size_t resp_cap
);

#endif
