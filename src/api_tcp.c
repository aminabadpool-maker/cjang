#define _POSIX_C_SOURCE 200809L
#include "api_tcp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static ssize_t read_chunk_retry(int fd, char *buf, size_t cap) {
    for (;;) {
        ssize_t r = read(fd, buf, cap);
        if (r < 0 && errno == EINTR) {
            continue;
        }
        return r;
    }
}

int cjang_serve_client_fd(
    int client_fd,
    const cjang_route_t *routes,
    size_t route_count
) {
    if (client_fd < 0) {
        return -1;
    }

    char req_buf[CJANG_TCP_REQ_CAP];
    char resp_buf[CJANG_TCP_RESP_CAP];
    char body_buf[CJANG_TCP_MAX_BODY + 1];

    size_t used = 0;
    char *hdr_end = NULL;

    while (used < sizeof(req_buf) - 1) {
        ssize_t r = read_chunk_retry(client_fd, req_buf + used, (sizeof(req_buf) - 1) - used);
        if (r <= 0) {
            break;
        }
        used += (size_t)r;
        req_buf[used] = '\0';

        hdr_end = strstr(req_buf, "\r\n\r\n");
        if (hdr_end != NULL) {
            break;
        }
    }

    if (used == 0) {
        return cjang_write_all_and_close(client_fd, NULL, 0);
    }

    if (hdr_end == NULL) {
        size_t rlen = cjang_api_build_http_response(
            resp_buf,
            sizeof(resp_buf),
            400,
            "Bad Request",
            "text/plain; charset=utf-8",
            "incomplete headers"
        );
        return cjang_write_all_and_close(client_fd, resp_buf, rlen);
    }

    size_t hdr_total_len = (size_t)((hdr_end + 4) - req_buf);
    *hdr_end = '\0'; /* null-terminate headers at \r\n\r\n for header parsing */

    size_t expected_body = 0;
    int cl_rc = cjang_parse_content_length(req_buf, CJANG_TCP_MAX_BODY, &expected_body);
    if (cl_rc == -2) {
        size_t rlen = cjang_api_build_http_response(
            resp_buf,
            sizeof(resp_buf),
            413,
            "Payload Too Large",
            "text/plain; charset=utf-8",
            "body exceeds limit"
        );
        return cjang_write_all_and_close(client_fd, resp_buf, rlen);
    }
    if (cl_rc < 0) {
        size_t rlen = cjang_api_build_http_response(
            resp_buf,
            sizeof(resp_buf),
            400,
            "Bad Request",
            "text/plain; charset=utf-8",
            "invalid content-length"
        );
        return cjang_write_all_and_close(client_fd, resp_buf, rlen);
    }

    size_t body_got = 0;
    if (expected_body > 0) {
        size_t already = (used > hdr_total_len) ? (used - hdr_total_len) : 0;
        size_t copy_init = already < expected_body ? already : expected_body;
        if (copy_init > 0) {
            memcpy(body_buf, req_buf + hdr_total_len, copy_init);
            body_got = copy_init;
        }

        while (body_got < expected_body) {
            ssize_t r = read_chunk_retry(client_fd, body_buf + body_got, expected_body - body_got);
            if (r <= 0) {
                break;
            }
            body_got += (size_t)r;
        }

        if (body_got < expected_body) {
            size_t rlen = cjang_api_build_http_response(
                resp_buf,
                sizeof(resp_buf),
                400,
                "Bad Request",
                "text/plain; charset=utf-8",
                "truncated body"
            );
            return cjang_write_all_and_close(client_fd, resp_buf, rlen);
        }
    }
    body_buf[body_got] = '\0';

    size_t rlen = cjang_dispatch_request(
        routes,
        route_count,
        req_buf,
        body_got > 0 ? body_buf : NULL,
        body_got,
        resp_buf,
        sizeof(resp_buf)
    );

    if (rlen == 0) {
        rlen = cjang_api_build_http_response(
            resp_buf,
            sizeof(resp_buf),
            500,
            "Internal Server Error",
            "text/plain; charset=utf-8",
            "response buffer error"
        );
    }

    return cjang_write_all_and_close(client_fd, resp_buf, rlen);
}

int cjang_tcp_listen_ipv4(const char *bind_ip, unsigned short port) {
    int srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_fd < 0) {
        return -1;
    }

    int opt = 1;
    (void)setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    const char *ip_str = (bind_ip && bind_ip[0] != '\0') ? bind_ip : "127.0.0.1";
    if (inet_pton(AF_INET, ip_str, &addr.sin_addr) != 1) {
        close(srv_fd);
        return -1;
    }

    if (bind(srv_fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(srv_fd);
        return -1;
    }

    if (listen(srv_fd, 64) != 0) {
        close(srv_fd);
        return -1;
    }

    return srv_fd;
}
