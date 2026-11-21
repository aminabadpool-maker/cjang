#include "cj_http_req_v1.h"
#include <string.h>

static void copy_span(const char *start, const char *end, char *out, size_t cap) {
    if (!out || cap == 0) return;
    size_t n = (end > start) ? (size_t)(end - start) : 0;
    if (n >= cap) n = cap - 1;
    if (n > 0) memcpy(out, start, n);
    out[n] = '\0';
}

int cj_http_parse_req_line(
    const char *req_buf,
    char *method_out, size_t method_cap,
    char *target_out, size_t target_cap
) {
    if (!req_buf || !method_out || method_cap == 0 || !target_out || target_cap == 0) {
        return -1;
    }
    method_out[0] = '\0';
    target_out[0] = '\0';

    const char *sp1 = strchr(req_buf, ' ');
    if (!sp1 || sp1 == req_buf) return -1;

    const char *tstart = sp1 + 1;
    const char *sp2 = strchr(tstart, ' ');
    if (!sp2 || sp2 == tstart) return -1;

    const char *qmark = tstart;
    while (qmark < sp2 && *qmark != '?') qmark++;

    copy_span(req_buf, sp1, method_out, method_cap);
    copy_span(tstart, qmark, target_out, target_cap);

    if (method_out[0] == '\0' || target_out[0] == '\0') return -1;
    return 0;
}
