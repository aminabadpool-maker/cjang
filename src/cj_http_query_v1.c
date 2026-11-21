#include "cj_http_query_v1.h"
#include "cj_url_decode_v1.h"
#include <string.h>

int cj_http_get_query_param(
    const char *req_buf,
    const char *key,
    char *val_out,
    size_t val_cap
) {
    if (!req_buf || !key || key[0] == '\0' || !val_out || val_cap == 0) return -1;
    val_out[0] = '\0';

    const char *sp1 = strchr(req_buf, ' ');
    if (!sp1) return -1;
    const char *qstart = strchr(sp1 + 1, '?');
    const char *sp2 = strchr(sp1 + 1, ' ');
    if (!qstart || !sp2 || qstart >= sp2) return -1;

    size_t klen = strlen(key);
    const char *p = qstart + 1;

    while (p < sp2) {
        const char *amp = p;
        while (amp < sp2 && *amp != '&') amp++;

        const char *eq = p;
        while (eq < amp && *eq != '=') eq++;

        if (eq < amp && (size_t)(eq - p) == klen && strncmp(p, key, klen) == 0) {
            size_t raw_len = (size_t)(amp - (eq + 1));
            char raw[512];
            if (raw_len >= sizeof(raw)) raw_len = sizeof(raw) - 1;
            if (raw_len > 0) memcpy(raw, eq + 1, raw_len);
            raw[raw_len] = '\0';

            cj_url_decode(raw, val_out, val_cap);
            return 0;
        }

        if (amp >= sp2) break;
        p = amp + 1;
    }

    return -1;
}
