#include "cj_portal_logout_v1.h"
#include <stdio.h>
#include <string.h>

size_t cj_portal_build_logout_resp(
    const char *cookie_name,
    const char *body,
    char *out_buf,
    size_t out_cap
) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    const char *cname = (cookie_name && cookie_name[0] != '\0') ? cookie_name : "cj_sid";
    const char *b = body ? body : "logged out";
    size_t blen = strlen(b);

    char hdr[768];
    int hlen = snprintf(
        hdr,
        sizeof(hdr),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Set-Cookie: %s=; Path=/; Max-Age=0; HttpOnly\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        cname,
        blen
    );
    if (hlen <= 0 || (size_t)hlen >= sizeof(hdr)) return 0;

    size_t total = (size_t)hlen + blen;
    if (total >= out_cap) return 0;

    memcpy(out_buf, hdr, (size_t)hlen);
    if (blen > 0) {
        memcpy(out_buf + hlen, b, blen);
    }
    out_buf[total] = '\0';
    return total;
}
