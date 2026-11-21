#include "cj_http_status_v1.h"
#include "cj_http_resp_v1.h"

size_t cj_http_build_400(char *resp_buf, size_t resp_cap) {
    return cj_http_build_text_resp(
        400,
        "Bad Request",
        "text/plain; charset=utf-8",
        "Bad Request",
        resp_buf,
        resp_cap
    );
}

size_t cj_http_build_405(char *resp_buf, size_t resp_cap) {
    return cj_http_build_text_resp(
        405,
        "Method Not Allowed",
        "text/plain; charset=utf-8",
        "Method Not Allowed",
        resp_buf,
        resp_cap
    );
}
