#include "cj_http_cookie_v1.h"
#include "cj_http_hdr_v1.h"
#include <string.h>

int cj_http_get_cookie(
    const char *req_buf,
    const char *name,
    char *val_out,
    size_t val_cap
) {
    if (!req_buf || !name || name[0] == '\0' || !val_out || val_cap == 0) return -1;
    val_out[0] = '\0';

    char hdr[1024];
    if (cj_http_get_header(req_buf, "Cookie", hdr, sizeof(hdr)) != 0) {
        return -1;
    }

    size_t nlen = strlen(name);
    const char *p = hdr;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t' || *p == ';') p++;
        if (*p == '\0') break;

        const char *semi = strchr(p, ';');
        const char *end = semi ? semi : (p + strlen(p));

        const char *eq = p;
        while (eq < end && *eq != '=') eq++;

        if (eq < end && (size_t)(eq - p) == nlen && strncmp(p, name, nlen) == 0) {
            const char *vstart = eq + 1;
            const char *vend = end;
            while (vend > vstart && (vend[-1] == ' ' || vend[-1] == '\t')) vend--;

            size_t vlen = (vend > vstart) ? (size_t)(vend - vstart) : 0;
            if (vlen >= val_cap) vlen = val_cap - 1;
            if (vlen > 0) memcpy(val_out, vstart, vlen);
            val_out[vlen] = '\0';
            return 0;
        }

        if (!semi) break;
        p = semi + 1;
    }

    return -1;
}
