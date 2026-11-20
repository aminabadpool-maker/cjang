#define _POSIX_C_SOURCE 200112L
#include "cjang.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECV_BUF_SIZE 4096

static void parse_first_line(const char *buf, char *method, size_t m_sz, char *path, size_t p_sz) {
    method[0] = '\0';
    path[0] = '\0';

    const char *sp1 = strchr(buf, ' ');
    if (!sp1) return;
    size_t m_len = (size_t)(sp1 - buf);
    if (m_len >= m_sz) m_len = m_sz - 1;
    memcpy(method, buf, m_len);
    method[m_len] = '\0';

    const char *p_start = sp1 + 1;
    const char *sp2 = strchr(p_start, ' ');
    if (!sp2) return;
    size_t p_len = (size_t)(sp2 - p_start);
    if (p_len >= p_sz) p_len = p_sz - 1;
    memcpy(path, p_start, p_len);
    path[p_len] = '\0';

    char *q = strchr(path, '?');
    if (q) *q = '\0';
}

int cjang_serve(const cjang_app_t *app, const char *host, int port) {
    int srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(srv_fd);
        return -1;
    }

    if (bind(srv_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(srv_fd);
        return -1;
    }

    if (listen(srv_fd, 64) < 0) {
        perror("listen");
        close(srv_fd);
        return -1;
    }

    printf("cjang listening on http://%s:%d\n", host, port);
    fflush(stdout);

    for (;;) {
        int client_fd = accept(srv_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buf[RECV_BUF_SIZE];
        ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = '\0';
            char method[16];
            char path[256];
            parse_first_line(buf, method, sizeof(method), path, sizeof(path));

            cjang_request_t req;
            memset(&req, 0, sizeof(req));
            req.method = method;
            req.path = path;
            cjang_response_t res = cjang_dispatch(app, &req);

            char header[512];
            size_t body_len = res.body ? strlen(res.body) : 0;
            int h_len = snprintf(
                header,
                sizeof(header),
                "HTTP/1.1 %d OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n\r\n",
                res.status,
                res.content_type ? res.content_type : "text/plain",
                body_len
            );

            if (h_len > 0) {
                send(client_fd, header, (size_t)h_len, 0);
            }
            if (body_len > 0) {
                send(client_fd, res.body, body_len, 0);
            }
        }

        close(client_fd);
    }
}
