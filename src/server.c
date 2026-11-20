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

static const char *trim_left_spaces(const char *s, const char *end) {
    while (s < end && (*s == ' ' || *s == '\t')) {
        s++;
    }
    return s;
}

static size_t trim_right_spaces(const char *s, size_t len) {
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\r')) {
        len--;
    }
    return len;
}

static void parse_first_line(const char *line_start, const char *line_end,
                             char *method, size_t m_sz,
                             char *path, size_t p_sz,
                             char *query, size_t q_sz) {
    method[0] = '\0';
    path[0] = '\0';
    if (query && q_sz > 0) query[0] = '\0';

    if (!line_start || !line_end || line_start >= line_end) return;

    const char *sp1 = memchr(line_start, ' ', (size_t)(line_end - line_start));
    if (!sp1) return;
    size_t m_len = (size_t)(sp1 - line_start);
    if (m_len >= m_sz) m_len = m_sz - 1;
    memcpy(method, line_start, m_len);
    method[m_len] = '\0';

    const char *p_start = sp1 + 1;
    const char *sp2 = memchr(p_start, ' ', (size_t)(line_end - p_start));
    if (!sp2) return;
    size_t p_len = (size_t)(sp2 - p_start);
    if (p_len >= p_sz) p_len = p_sz - 1;
    memcpy(path, p_start, p_len);
    path[p_len] = '\0';

    char *q = strchr(path, '?');
    if (q) {
        *q = '\0';
        if (query && q_sz > 0) {
            size_t q_len = strlen(q + 1);
            if (q_len >= q_sz) q_len = q_sz - 1;
            memcpy(query, q + 1, q_len);
            query[q_len] = '\0';
        }
    }
}

static void parse_headers(cjang_request_t *req, const char *hdr_start, const char *hdr_end) {
    req->header_count = 0;
    if (!req || !hdr_start || !hdr_end || hdr_start >= hdr_end) return;

    const char *cur = hdr_start;
    while (cur < hdr_end && req->header_count < CJANG_MAX_HEADERS) {
        const char *nl = memchr(cur, '\n', (size_t)(hdr_end - cur));
        const char *line_end = nl ? nl : hdr_end;

        size_t raw_len = (size_t)(line_end - cur);
        raw_len = trim_right_spaces(cur, raw_len);

        if (raw_len > 0) {
            const char *colon = memchr(cur, ':', raw_len);
            if (colon) {
                size_t n_len = (size_t)(colon - cur);
                const char *v_start = trim_left_spaces(colon + 1, cur + raw_len);
                size_t v_len = (size_t)((cur + raw_len) - v_start);
                v_len = trim_right_spaces(v_start, v_len);

                if (n_len > 0 && n_len < CJANG_HEADER_NAME_LEN && v_len < CJANG_HEADER_VAL_LEN) {
                    cjang_header_t *h = &req->headers[req->header_count];
                    memcpy(h->name, cur, n_len);
                    h->name[n_len] = '\0';
                    memcpy(h->value, v_start, v_len);
                    h->value[v_len] = '\0';
                    req->header_count++;
                }
            }
        }

        if (!nl) break;
        cur = nl + 1;
    }
}

static const char *find_header_end(const char *buf, size_t len, size_t *sep_len) {
    for (size_t i = 0; i + 3 < len; ++i) {
        if (buf[i] == '\r' && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n') {
            *sep_len = 4;
            return buf + i + 4;
        }
    }
    for (size_t i = 0; i + 1 < len; ++i) {
        if (buf[i] == '\n' && buf[i + 1] == '\n') {
            *sep_len = 2;
            return buf + i + 2;
        }
    }
    return NULL;
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
        size_t total_read = 0;
        size_t sep_len = 0;
        const char *body_start = NULL;

        while (total_read < sizeof(buf) - 1) {
            ssize_t n = recv(client_fd, buf + total_read, sizeof(buf) - 1 - total_read, 0);
            if (n <= 0) break;
            total_read += (size_t)n;
            buf[total_read] = '\0';

            body_start = find_header_end(buf, total_read, &sep_len);
            if (body_start) {
                break;
            }
        }

        if (total_read > 0) {
            char method[16];
            char path[256];
            char raw_query[512];

            const char *first_nl = memchr(buf, '\n', total_read);
            const char *first_end = first_nl ? first_nl : (buf + total_read);
            parse_first_line(buf, first_end,
                             method, sizeof(method),
                             path, sizeof(path),
                             raw_query, sizeof(raw_query));

            cjang_request_t req;
            memset(&req, 0, sizeof(req));
            req.method = method;
            req.path = path;
            cjang_parse_query(&req, raw_query);

            if (body_start && first_nl && (first_nl + 1) < (body_start - sep_len)) {
                parse_headers(&req, first_nl + 1, body_start - sep_len);
            }

            size_t content_len = 0;
            const char *cl_hdr = cjang_header_get(&req, "Content-Length");
            if (cl_hdr && *cl_hdr != '\0') {
                long parsed_cl = strtol(cl_hdr, NULL, 10);
                if (parsed_cl > 0) {
                    content_len = (size_t)parsed_cl;
                    if (content_len >= CJANG_MAX_BODY_LEN) {
                        content_len = CJANG_MAX_BODY_LEN - 1;
                    }
                }
            }

            if (body_start && content_len > 0) {
                size_t already_in_buf = total_read - (size_t)(body_start - buf);
                size_t to_copy = already_in_buf < content_len ? already_in_buf : content_len;
                if (to_copy > 0) {
                    memcpy(req.body, body_start, to_copy);
                    req.body_len = to_copy;
                }

                while (req.body_len < content_len && total_read < sizeof(buf) - 1) {
                    ssize_t n = recv(client_fd, buf + total_read, sizeof(buf) - 1 - total_read, 0);
                    if (n <= 0) break;
                    size_t avail = (size_t)n;
                    size_t needed = content_len - req.body_len;
                    size_t take = avail < needed ? avail : needed;
                    memcpy(req.body + req.body_len, buf + total_read, take);
                    req.body_len += take;
                    total_read += avail;
                }
            } else if (body_start) {
                size_t rem = total_read - (size_t)(body_start - buf);
                if (rem >= CJANG_MAX_BODY_LEN) rem = CJANG_MAX_BODY_LEN - 1;
                if (rem > 0) {
                    memcpy(req.body, body_start, rem);
                    req.body_len = rem;
                }
            }
            req.body[req.body_len] = '\0';

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
