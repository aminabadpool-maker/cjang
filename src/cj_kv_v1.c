#include "cj_kv_v1.h"
#include <string.h>

static const char *skip_ws(const char *p) {
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
        p++;
    }
    return p;
}

static void copy_trimmed(const char *start, const char *end, char *out, size_t cap) {
    if (!out || cap == 0) return;
    while (end > start && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n')) {
        end--;
    }
    size_t n = (end > start) ? (size_t)(end - start) : 0;
    if (n >= cap) n = cap - 1;
    if (n > 0) memcpy(out, start, n);
    out[n] = '\0';
}

int cj_kv_parse_line(const char *line, char *key_out, size_t key_cap, char *val_out, size_t val_cap) {
    if (!line || !key_out || key_cap == 0 || !val_out || val_cap == 0) return -1;
    key_out[0] = '\0';
    val_out[0] = '\0';

    const char *p = skip_ws(line);
    if (*p == '\0' || *p == '#') return 1;

    const char *eq = strchr(p, '=');
    if (!eq || eq == p) return -1;

    copy_trimmed(p, eq, key_out, key_cap);
    if (key_out[0] == '\0') return -1;

    const char *vstart = eq + 1;
    while (*vstart == ' ' || *vstart == '\t') vstart++;

    const char *vend = vstart;
    while (*vend != '\0' && *vend != '\r' && *vend != '\n') vend++;

    copy_trimmed(vstart, vend, val_out, val_cap);
    return 0;
}

int cj_kv_get_from_buf(const char *buf, const char *key, char *val_out, size_t val_cap) {
    if (!buf || !key || key[0] == '\0' || !val_out || val_cap == 0) return -1;
    val_out[0] = '\0';

    const char *cur = buf;
    while (*cur != '\0') {
        const char *nl = strchr(cur, '\n');
        size_t line_len = nl ? (size_t)(nl - cur) : strlen(cur);

        char line[CJ_KV_KEY_MAX + CJ_KV_VAL_MAX + 8];
        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, cur, line_len);
        line[line_len] = '\0';

        char k[CJ_KV_KEY_MAX];
        char v[CJ_KV_VAL_MAX];
        if (cj_kv_parse_line(line, k, sizeof(k), v, sizeof(v)) == 0) {
            if (strcmp(k, key) == 0) {
                size_t vlen = strlen(v);
                if (vlen >= val_cap) vlen = val_cap - 1;
                memcpy(val_out, v, vlen);
                val_out[vlen] = '\0';
                return 0;
            }
        }

        if (!nl) break;
        cur = nl + 1;
    }

    return -1;
}
