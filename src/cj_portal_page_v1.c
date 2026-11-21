#include "cj_portal_page_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_render_html(
    const char *title,
    const char *heading,
    const char *body_text,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;

    const char *t = (title && title[0] != '\0') ? title : "cjang";
    const char *h = (heading && heading[0] != '\0') ? heading : "cjang portal";
    const char *b = body_text ? body_text : "";

    int n = snprintf(
        out_buf,
        out_cap,
        "<!doctype html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"utf-8\" />\n"
        "  <title>%s</title>\n"
        "  <link rel=\"stylesheet\" href=\"/style.css\" />\n"
        "</head>\n"
        "<body>\n"
        "  <h1>%s</h1>\n"
        "  <p>%s</p>\n"
        "</body>\n"
        "</html>\n",
        t, h, b
    );

    if (n < 0) {
        out_buf[0] = '\0';
        return 0;
    }
    if ((size_t)n >= out_cap) {
        out_buf[out_cap - 1] = '\0';
        return out_cap - 1;
    }
    return (size_t)n;
}
