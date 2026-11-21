#ifndef CJ_PORTAL_PAGE_V1_H
#define CJ_PORTAL_PAGE_V1_H

#include <stddef.h>

#ifndef CJ_PORTAL_PAGE_MAX
#define CJ_PORTAL_PAGE_MAX 8192u
#endif

size_t cj_portal_render_html(
    const char *title,
    const char *heading,
    const char *body_text,
    char *out_buf,
    size_t out_cap
);

#endif
