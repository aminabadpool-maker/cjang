#include "cjang_guard.h"

#include <string.h>

const char *cjang_guard_strerror(cjang_guard_err_t err) {
    switch (err) {
        case CJANG_GUARD_OK:
            return "ok";
        case CJANG_GUARD_ERR_NULL:
            return "null_or_empty_input";
        case CJANG_GUARD_ERR_BOUNDS:
            return "invalid_bounds_or_capacity";
        case CJANG_GUARD_ERR_TOO_LONG:
            return "exceeds_length_limit";
        case CJANG_GUARD_ERR_SYNTAX:
            return "malformed_syntax_or_empty_value";
        case CJANG_GUARD_ERR_NOT_FOUND:
            return "key_not_found";
        default:
            return "unknown_guard_error";
    }
}

cjang_guard_err_t cjang_guard_validate_buf(
    const char *buf,
    size_t len,
    size_t max_len
) {
    if (!buf) {
        return CJANG_GUARD_ERR_NULL;
    }
    if (max_len == 0 || len == 0) {
        return CJANG_GUARD_ERR_BOUNDS;
    }
    if (len > max_len) {
        return CJANG_GUARD_ERR_TOO_LONG;
    }
    return CJANG_GUARD_OK;
}

static int is_ws(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static int is_delim(char c) {
    return c == ',' || c == '}' || c == ']' || c == ';' ||
           c == '\n' || c == '\r' || c == ' ' || c == '\t';
}

cjang_guard_err_t cjang_guard_extract_kv(
    const char *buf,
    size_t len,
    const char *key,
    char *out,
    size_t out_cap
) {
    if (!out) {
        return CJANG_GUARD_ERR_NULL;
    }
    if (out_cap == 0) {
        return CJANG_GUARD_ERR_BOUNDS;
    }
    out[0] = '\0';

    if (!key || key[0] == '\0') {
        return CJANG_GUARD_ERR_NULL;
    }

    size_t klen = strlen(key);
    if (klen > CJANG_GUARD_MAX_KEY) {
        return CJANG_GUARD_ERR_TOO_LONG;
    }

    cjang_guard_err_t brc = cjang_guard_validate_buf(buf, len, CJANG_GUARD_MAX_BUF);
    if (brc != CJANG_GUARD_OK) {
        return brc;
    }

    if (out_cap - 1 > CJANG_GUARD_MAX_VAL) {
        out_cap = CJANG_GUARD_MAX_VAL + 1;
    }

    if (len < klen) {
        return CJANG_GUARD_ERR_NOT_FOUND;
    }

    size_t i = 0;
    while (i + klen < len) {
        if (memcmp(buf + i, key, klen) == 0) {
            int quoted_key = (i > 0 && buf[i - 1] == '"') ? 1 : 0;

            size_t sep = i + klen;
            if (sep < len && buf[sep] == '"') {
                if (!quoted_key) {
                    i++;
                    continue;
                }
                sep++;
            } else if (quoted_key) {
                i++;
                continue;
            }

            while (sep < len && is_ws(buf[sep])) {
                sep++;
            }

            if (sep >= len || (buf[sep] != ':' && buf[sep] != '=')) {
                i++;
                continue;
            }

            size_t p = sep + 1;
            while (p < len && (buf[p] == ' ' || buf[p] == '\t')) {
                p++;
            }
            if (p >= len) {
                return CJANG_GUARD_ERR_SYNTAX;
            }

            size_t vstart = p;
            size_t vend = vstart;

            if (buf[p] == '"') {
                vstart++;
                vend = vstart;
                while (vend < len && buf[vend] != '"') {
                    if (buf[vend] == '\\' && vend + 1 < len) {
                        vend += 2;
                        continue;
                    }
                    vend++;
                }
                if (vend >= len) {
                    return CJANG_GUARD_ERR_SYNTAX;
                }
            } else {
                while (vend < len && !is_delim(buf[vend])) {
                    vend++;
                }
            }

            size_t vlen = vend - vstart;
            if (vlen == 0) {
                return CJANG_GUARD_ERR_SYNTAX;
            }
            if (vlen > CJANG_GUARD_MAX_VAL || vlen + 1 > out_cap) {
                return CJANG_GUARD_ERR_TOO_LONG;
            }

            memcpy(out, buf + vstart, vlen);
            out[vlen] = '\0';
            return CJANG_GUARD_OK;
        }
        i++;
    }

    return CJANG_GUARD_ERR_NOT_FOUND;
}
