#include "api_agent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_bounded_str(char *dst, size_t cap, const char *src, size_t len) {
    if (!dst || cap == 0) {
        return;
    }
    if (!src || len == 0) {
        dst[0] = '\0';
        return;
    }
    size_t n = len < (cap - 1) ? len : (cap - 1);
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static int extract_kv_token(const char *buf, size_t len, const char *key, char *out, size_t out_cap) {
    if (!buf || !key || !out || out_cap == 0) {
        return -1;
    }
    size_t klen = strlen(key);
    if (klen == 0 || len < klen) {
        return -1;
    }

    size_t i = 0;
    while (i + klen <= len) {
        if (memcmp(buf + i, key, klen) == 0) {
            size_t p = i + klen;
            while (p < len && (buf[p] == ' ' || buf[p] == '\t' || buf[p] == '"')) {
                p++;
            }
            if (p < len && (buf[p] == '=' || buf[p] == ':')) {
                size_t vstart = p + 1;
                while (vstart < len && (buf[vstart] == ' ' || buf[vstart] == '\t' || buf[vstart] == '"')) {
                    vstart++;
                }
                size_t vend = vstart;
                while (vend < len && buf[vend] != '"' && buf[vend] != ',' &&
                       buf[vend] != '&' && buf[vend] != ';' && buf[vend] != '\n' &&
                       buf[vend] != '\r' && buf[vend] != '}') {
                    vend++;
                }
                copy_bounded_str(out, out_cap, buf + vstart, vend - vstart);
                return 0;
            }
        }
        i++;
    }
    return -1;
}

static uint64_t extract_seq_val(const char *buf, size_t len) {
    char tmp[32];
    if (extract_kv_token(buf, len, "seq", tmp, sizeof(tmp)) == 0 && tmp[0] != '\0') {
        char *endp = NULL;
        unsigned long long v = strtoull(tmp, &endp, 10);
        if (endp != tmp) {
            return (uint64_t)v;
        }
    }
    return 0;
}

int cjang_agent_parse_msg(const char *payload, size_t len, cjang_agent_msg_t *out) {
    if (!payload || len == 0 || !out) {
        return -1;
    }
    memset(out, 0, sizeof(*out));

    if (extract_kv_token(payload, len, "agent", out->agent_id, sizeof(out->agent_id)) != 0) {
        (void)extract_kv_token(payload, len, "agent_id", out->agent_id, sizeof(out->agent_id));
    }
    if (extract_kv_token(payload, len, "cmd", out->cmd, sizeof(out->cmd)) != 0) {
        return -1;
    }
    if (out->cmd[0] == '\0') {
        return -1;
    }
    out->seq = extract_seq_val(payload, len);
    return 0;
}

size_t cjang_agent_handle_http(
    const cjang_metrics_t *m,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
) {
    if (!raw_req || !http_out || http_cap == 0) {
        return 0;
    }

    uint64_t total_req = m ? m->total_requests : 0;
    uint64_t ok_200 = m ? m->ok_200 : 0;
    uint64_t last_us = m ? m->last_exec_us : 0;

    if (strncmp(raw_req, "GET /__agent/status", 19) == 0) {
        char kv_body[256];
        int blen = snprintf(
            kv_body,
            sizeof(kv_body),
            "engine=cjang\nproto=agent-v1\nstatus=ready\ntotal_req=%llu\nok_200=%llu\nlast_us=%llu\n",
            (unsigned long long)total_req,
            (unsigned long long)ok_200,
            (unsigned long long)last_us
        );
        if (blen < 0) {
            return 0;
        }
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            200,
            "OK",
            "text/plain; charset=utf-8",
            kv_body
        );
    }

    if (strncmp(raw_req, "POST /__agent/exec", 18) == 0) {
        cjang_agent_msg_t msg;
        if (cjang_agent_parse_msg(body, body_len, &msg) != 0) {
            return cjang_api_build_http_response(
                http_out,
                http_cap,
                400,
                "Bad Request",
                "text/plain; charset=utf-8",
                "result=err\nreason=invalid_agent_payload\n"
            );
        }

        const char *agent_label = msg.agent_id[0] != '\0' ? msg.agent_id : "anon";
        char ack_body[256];
        int blen = snprintf(
            ack_body,
            sizeof(ack_body),
            "result=ok\nagent=%s\ncmd=%s\nseq=%llu\nlast_us=%llu\n",
            agent_label,
            msg.cmd,
            (unsigned long long)msg.seq,
            (unsigned long long)last_us
        );
        if (blen < 0) {
            return 0;
        }
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            200,
            "OK",
            "text/plain; charset=utf-8",
            ack_body
        );
    }

    return cjang_api_build_http_response(
        http_out,
        http_cap,
        404,
        "Not Found",
        "text/plain; charset=utf-8",
        "result=err\nreason=unknown_agent_endpoint\n"
    );
}
