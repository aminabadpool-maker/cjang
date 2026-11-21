#include "cj_http_hdr_v1.h"
#include <string.h>

static int ieq_n(const char *a, const char *b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca >= 'A' && ca <= 'Z') ca = (unsigned char)(ca + ('a' - 'A'));
        if (cb >= 'A' && cb <= 'Z') cb = (unsigned char)(cb + ('a' - 'A'));
        if (ca != cb) return 0;
    }
    return 1;
}

int cj_http_get_header(const char *req_buf, const char *name, char *val_out, size_t val_cap) {
    if (!req_buf || !name || name[0] == '\0' || !val_out || val_cap == 0) return -1;
    val_out[0] = '\0';

    size_t nlen = strlen(name);
    const char *line = strstr(req_buf, "\r\n");
    if (!line) return -1;
    line += 2;

    while (*line != '\0') {
        if (line[0] == '\r' && line[1] == '\n') break;
        if (line[0] == '\n') break;

        const char *eol = strstr(line, "\r\n");
        const char *next = NULL;
        if (eol) {
            next = eol + 2;
        } else {
            const char *nl = strchr(line, '\n');
            eol = nl ? nl : (line + strlen(line));
            next = nl ? (nl + 1) : eol;
        }

        const char *colon = line;
        while (colon < eol && *colon != ':') colon++;

        if (colon < eol && (size_t)(colon - line) == nlen && ieq_n(line, name, nlen)) {
            const char *vstart = colon + 1;
            while (vstart < eol && (*vstart == ' ' || *vstart == '\t')) vstart++;

            const char *vend = eol;
            while (vend > vstart && (vend[-1] == ' ' || vend[-1] == '\t')) vend--;

            size_t vlen = (vend > vstart) ? (size_t)(vend - vstart) : 0;
            if (vlen >= val_cap) vlen = val_cap - 1;
            if (vlen > 0) memcpy(val_out, vstart, vlen);
            val_out[vlen] = '\0';
            return 0;
        }

        if (next == eol && *eol == '\0') break;
        line = next;
    }

    return -1;
}
