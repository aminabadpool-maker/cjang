#include "cj_portal_resp_v1.h"
#include "cj_portal_page_v1.h"
#include "cj_http_resp_v1.h"

size_t cj_portal_build_http_200(
    const char *title,
    const char *heading,
    const char *body_text,
    char *resp_buf,
    size_t resp_cap
) {
    char html[CJ_PORTAL_PAGE_MAX];
    size_t hlen = cj_portal_render_html(title, heading, body_text, html, sizeof(html));
    if (hlen == 0) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "text/html; charset=utf-8",
        html,
        resp_buf,
        resp_cap
    );
}
