#define _POSIX_C_SOURCE 200809L
#include "api_agent.h"
#include "api_codec.h"
#include "api_ext.h"
#include "api_metrics.h"
#include "api_mw.h"
#include "api_tcp.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static int client_roundtrip_127(unsigned short port, const char *req, char *resp, size_t resp_cap) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
        close(fd);
        return -1;
    }

    if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(fd);
        return -1;
    }

    size_t rlen = strlen(req);
    size_t sent = 0;
    while (sent < rlen) {
        ssize_t w = send(fd, req + sent, rlen - sent, 0);
        if (w <= 0) {
            close(fd);
            return -1;
        }
        sent += (size_t)w;
    }

    size_t total = 0;
    while (total + 1 < resp_cap) {
        ssize_t r = recv(fd, resp + total, (resp_cap - 1) - total, 0);
        if (r <= 0) {
            break;
        }
        total += (size_t)r;
    }
    resp[total] = '\0';
    close(fd);
    return (int)total;
}

static void test_codec_and_sigpipe_stress(void) {
    assert(cjang_ignore_sigpipe() == 0);

    char out[256];
    const char *enc = "query=hello+world%21+%D9%81%D8%A7%D8%B1%D8%B3%DB%8C%2Fbad%2G%2";
    size_t n = cjang_url_decode(enc, strlen(enc), out, sizeof(out));
    assert(n > 0);
    assert(strstr(out, "query=hello world!") != NULL);
    assert(strstr(out, "/bad%2G%2") != NULL);

    /* Repeat broken-pipe writes to ensure zero SIGPIPE termination */
    for (int i = 0; i < 64; i++) {
        int sv[2];
        assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
        close(sv[0]);
        int rc = cjang_send_all_nosig_and_close(sv[1], "payload_chunk", 13);
        assert(rc == -1);
    }
}

static void test_ext_and_mw_guards(void) {
    const char *req =
        "POST /api/v1/items HTTP/1.1\r\n"
        "Host: agent-node.local\r\n"
        "X-Agent-Token: secret-token-01\r\n"
        "Cookie: sid=s99; role=admin\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\"cmd\":\"sync\",\"seq\":128}";

    char hdr[64];
    assert(cjang_ext_header_get(req, "x-agent-token", hdr, sizeof(hdr)) > 0);
    assert(strcmp(hdr, "secret-token-01") == 0);

    char cookie[64];
    assert(cjang_ext_cookie_get(req, "role", cookie, sizeof(cookie)) > 0);
    assert(strcmp(cookie, "admin") == 0);

    assert(cjang_ext_method_allowed("POST", "GET, POST, HEAD") == 1);
    assert(cjang_ext_method_allowed("DELETE", "GET, POST, HEAD") == 0);

    char clean[128];
    assert(cjang_ext_path_clean("/a//b/./items?limit=10", clean, sizeof(clean)) == 0);
    assert(strcmp(clean, "/a/b/items") == 0);
    assert(cjang_ext_path_clean("/a/../../etc/passwd", clean, sizeof(clean)) == -1);

    char esc[128];
    assert(cjang_ext_json_escape("line1\nline2\t\"q\"\\", esc, sizeof(esc)) > 0);
    assert(strcmp(esc, "line1\\nline2\\t\\\"q\\\"\\\\") == 0);

    const char *body = strstr(req, "\r\n\r\n") + 4;
    int64_t seq = 0;
    assert(cjang_ext_json_get_int64(body, strlen(body), "seq", &seq) == 0 && seq == 128);

    cjang_ext_rate_t rl;
    cjang_ext_rate_init(&rl, 3, 60);
    assert(cjang_ext_rate_allow(&rl) == 1);
    assert(cjang_ext_rate_allow(&rl) == 1);
    assert(cjang_ext_rate_allow(&rl) == 1);
    assert(cjang_ext_rate_allow(&rl) == 0);

    char k[32], v[32];
    assert(cjang_ext_parse_kv_line("worker_id = 7\r\n", k, sizeof(k), v, sizeof(v)) == 0);
    assert(strcmp(k, "worker_id") == 0 && strcmp(v, "7") == 0);

    const cjang_route_t routes[] = {
        { "GET", "/health", NULL, "healthy" }
    };
    cjang_metrics_t m;
    cjang_metrics_reset(&m);

    uint64_t seen = 0;
    cjang_mw_hdr_guard_t guard = {
        .header_line_prefix = "X-Agent-Token: secret-token-01",
        .deny_body = "unauthorized\n"
    };
    cjang_mw_chain_t chain;
    cjang_mw_init(&chain);
    assert(cjang_mw_add(&chain, cjang_mw_count_req, &seen) == 0);
    assert(cjang_mw_add(&chain, cjang_mw_require_header_line, &guard) == 0);

    char mw_out[1024];
    size_t n_bad = cjang_mw_dispatch_metered(
        &chain, &m, routes, 1,
        "GET /health HTTP/1.1\r\nHost: agent-node.local\r\n\r\n",
        NULL, 0, mw_out, sizeof(mw_out)
    );
    assert(n_bad > 0 && strstr(mw_out, "HTTP/1.1 401 Unauthorized\r\n") == mw_out);

    const char *ok_req =
        "GET /health HTTP/1.1\r\n"
        "Host: agent-node.local\r\n"
        "X-Agent-Token: secret-token-01\r\n\r\n";
    size_t n_good = cjang_mw_dispatch_metered(
        &chain, &m, routes, 1, ok_req, NULL, 0, mw_out, sizeof(mw_out)
    );
    assert(n_good > 0 && strstr(mw_out, "HTTP/1.1 200 OK\r\n") == mw_out);
    assert(seen == 2 && m.client_4xx == 1 && m.ok_200 == 1);

    char snap[256];
    assert(cjang_ext_metrics_snapshot_kv(&m, snap, sizeof(snap)) > 0);
    assert(strstr(snap, "total_requests=2\n") != NULL);
}

static void test_loopback_127_burst(void) {
    int listen_fd = cjang_tcp_listen_ipv4("127.0.0.1", 0);
    assert(listen_fd >= 0);

    struct sockaddr_in bound;
    socklen_t blen = sizeof(bound);
    assert(getsockname(listen_fd, (struct sockaddr *)&bound, &blen) == 0);
    unsigned short port = ntohs(bound.sin_port);
    assert(port > 0);

    const cjang_route_t routes[] = {
        { "GET", "/ping", NULL, "pong_127" }
    };
    cjang_metrics_t metrics;
    cjang_metrics_reset(&metrics);

    const int burst_count = 16;
    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        close(listen_fd);
        char resp[2048];
        for (int i = 0; i < burst_count; i++) {
            const char *req =
                "GET /ping HTTP/1.1\r\n"
                "Host: 127.0.0.1\r\n"
                "Connection: close\r\n\r\n";
            int n = client_roundtrip_127(port, req, resp, sizeof(resp));
            assert(n > 0);
            assert(strstr(resp, "HTTP/1.1 200 OK\r\n") == resp);
            assert(strstr(resp, "pong_127") != NULL);
        }
        _exit(0);
    }

    for (int i = 0; i < burst_count; i++) {
        int cli = accept(listen_fd, NULL, NULL);
        assert(cli >= 0);
        assert(cjang_serve_client_fd(cli, routes, 1) == 0);
        metrics.total_requests++;
        metrics.ok_200++;
    }

    int status = 0;
    assert(waitpid(pid, &status, 0) == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0);
    close(listen_fd);

    assert(metrics.total_requests == (uint64_t)burst_count);
    assert(metrics.ok_200 == (uint64_t)burst_count);
}

int main(void) {
    test_codec_and_sigpipe_stress();
    test_ext_and_mw_guards();
    test_loopback_127_burst();
    puts("[OK] test_cjang_heavy passed");
    return 0;
}
