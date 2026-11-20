#define _POSIX_C_SOURCE 200809L
#include "api_tcp.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    const cjang_route_t routes[] = {
        { "POST", "/api/echo", NULL, "user={{user}} role={{role}}" },
        { "GET",  "/ping",     NULL, "pong" }
    };
    const size_t rcount = sizeof(routes) / sizeof(routes[0]);

    int srv_fd = cjang_tcp_listen_ipv4("127.0.0.1", 0);
    assert(srv_fd >= 0);

    struct sockaddr_in bound;
    socklen_t blen = sizeof(bound);
    assert(getsockname(srv_fd, (struct sockaddr *)&bound, &blen) == 0);
    uint16_t port = ntohs(bound.sin_port);

    int cli_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(cli_fd >= 0);

    struct sockaddr_in target;
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    assert(inet_pton(AF_INET, "127.0.0.1", &target.sin_addr) == 1);
    assert(connect(cli_fd, (const struct sockaddr *)&target, sizeof(target)) == 0);

    const char *body = "{\"user\":\"amin\",\"role\":\"c11\"}";
    char req[512];
    int req_n = snprintf(
        req,
        sizeof(req),
        "POST /api/echo HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        strlen(body),
        body
    );
    assert(req_n > 0 && (size_t)req_n < sizeof(req));

    ssize_t sent = write(cli_fd, req, (size_t)req_n);
    assert(sent == req_n);

    int acc_fd = accept(srv_fd, NULL, NULL);
    assert(acc_fd >= 0);
    assert(cjang_serve_client_fd(acc_fd, routes, rcount) == 0);

    char reply[1024];
    ssize_t recvd = read(cli_fd, reply, sizeof(reply) - 1);
    assert(recvd > 0);
    reply[recvd] = '\0';

    assert(strstr(reply, "HTTP/1.1 200 OK\r\n") == reply);
    assert(strstr(reply, "\r\n\r\nuser=amin role=c11") != NULL);

    close(cli_fd);
    close(srv_fd);

    printf("[OK] api_tcp loopback test passed (reply_bytes=%zd)\n", recvd);
    return 0;
}
