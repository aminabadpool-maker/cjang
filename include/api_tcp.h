#ifndef CJANG_API_TCP_H
#define CJANG_API_TCP_H

#include "api_router.h"
#include <stddef.h>

#ifndef CJANG_TCP_REQ_CAP
#define CJANG_TCP_REQ_CAP 4096
#endif

#ifndef CJANG_TCP_RESP_CAP
#define CJANG_TCP_RESP_CAP 4096
#endif

#ifndef CJANG_TCP_MAX_BODY
#define CJANG_TCP_MAX_BODY 2048
#endif

/* Reads request from client_fd, dispatches via routes, writes response, and closes client_fd. */
int cjang_serve_client_fd(
    int client_fd,
    const cjang_route_t *routes,
    size_t route_count
);

/* Creates a non-blocking/reusable IPv4 TCP listening socket on port (0 = ephemeral). */
int cjang_tcp_listen_ipv4(const char *bind_ip, unsigned short port);

#endif
