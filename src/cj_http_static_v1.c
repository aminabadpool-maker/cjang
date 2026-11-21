#include "cj_http_static_v1.h"
#include "cj_static_v1.h"
#include "cj_log_v1.h"
#include <stdio.h>
#include <string.h>

int cj_http_build_static_200(const char *root_dir, const char *relpath, char *resp_buf, size_t resp_cap, size_t *out_len) {
    if (!resp_buf || resp_cap < 128) return -1;

    char body[CJ_STATIC_BODY_MAX];
    size_t body_len = 0;
    const char *mime = NULL;

    if (cj_static_read_file(root_dir, relpath, body, sizeof(body), &body_len, &mime) != 0) {
        cj_log_write(CJ_LOG_WARN, "static", "file read failed or unsafe path");
        return -1;
    }

    char header[512];
    int hlen = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        mime ? mime : "application/octet-stream",
        body_len
    );
    if (hlen <= 0 || (size_t)hlen >= sizeof(header)) return -1;
    if ((size_t)hlen + body_len >= resp_cap) return -1;

    memcpy(resp_buf, header, (size_t)hlen);
    if (body_len > 0) {
        memcpy(resp_buf + hlen, body, body_len);
    }
    resp_buf[(size_t)hlen + body_len] = '\0';

    if (out_len) *out_len = (size_t)hlen + body_len;
    return 0;
}

size_t cj_http_build_404(char *resp_buf, size_t resp_cap) {
    static const char *not_found =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Not Found";

    if (!resp_buf || resp_cap == 0) return 0;
    size_t len = strlen(not_found);
    if (len >= resp_cap) len = resp_cap - 1;
    memcpy(resp_buf, not_found, len);
    resp_buf[len] = '\0';
    return len;
}
