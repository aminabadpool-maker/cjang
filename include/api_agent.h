#ifndef CJANG_API_AGENT_H
#define CJANG_API_AGENT_H

#include "api_metrics.h"
#include <stddef.h>
#include <stdint.h>

#ifndef CJANG_AGENT_ID_CAP
#define CJANG_AGENT_ID_CAP 64
#endif

#ifndef CJANG_AGENT_CMD_CAP
#define CJANG_AGENT_CMD_CAP 64
#endif

typedef struct {
    char agent_id[CJANG_AGENT_ID_CAP];
    char cmd[CJANG_AGENT_CMD_CAP];
    uint64_t seq;
} cjang_agent_msg_t;

/* Parses compact line/kv or flat JSON payload into cjang_agent_msg_t.
 * Returns 0 on valid parse, -1 on invalid input. */
int cjang_agent_parse_msg(const char *payload, size_t len, cjang_agent_msg_t *out);

/* Builds a deterministic plain-text/kv HTTP response tailored for agents. */
size_t cjang_agent_handle_http(
    const cjang_metrics_t *m,
    const char *raw_req,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
);

#endif
