#include "api_json.h"

#include <ctype.h>
#include <string.h>

static size_t skip_ws(const char *s, size_t pos, size_t len) {
    while (pos < len && (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\r' || s[pos] == '\n')) {
        pos++;
    }
    return pos;
}

static int parse_json_str(
    const char *s,
    size_t len,
    size_t *io_pos,
    char *dst,
    size_t dst_cap
) {
    size_t pos = *io_pos;
    if (pos >= len || s[pos] != '"') {
        return -1;
    }
    pos++; /* skip opening '"' */

    size_t w = 0;
    const size_t max_w = (dst && dst_cap > 0) ? (dst_cap - 1) : 0;

    while (pos < len) {
        char ch = s[pos++];
        if (ch == '"') {
            if (dst && dst_cap > 0) {
                dst[w] = '\0';
            }
            *io_pos = pos;
            return 0;
        }
        if (ch == '\\') {
            if (pos >= len) {
                return -1;
            }
            char esc = s[pos++];
            switch (esc) {
                case '"':  ch = '"';  break;
                case '\\': ch = '\\'; break;
                case '/':  ch = '/';  break;
                case 'n':  ch = '\n'; break;
                case 'r':  ch = '\r'; break;
                case 't':  ch = '\t'; break;
                default:   ch = esc;  break;
            }
        }
        if (dst && w < max_w) {
            dst[w++] = ch;
        }
    }

    return -1; /* unterminated string */
}

static int parse_json_scalar(
    const char *s,
    size_t len,
    size_t *io_pos,
    char *dst,
    size_t dst_cap
) {
    size_t pos = skip_ws(s, *io_pos, len);
    if (pos >= len) {
        return -1;
    }

    if (s[pos] == '"') {
        int rc = parse_json_str(s, len, &pos, dst, dst_cap);
        if (rc != 0) {
            return -1;
        }
        *io_pos = pos;
        return 0;
    }

    size_t start = pos;
    while (pos < len && s[pos] != ',' && s[pos] != '}' &&
           s[pos] != ' ' && s[pos] != '\t' && s[pos] != '\r' && s[pos] != '\n') {
        pos++;
    }
    size_t tlen = pos - start;
    if (tlen == 0) {
        return -1;
    }

    size_t w = 0;
    const size_t max_w = (dst && dst_cap > 0) ? (dst_cap - 1) : 0;
    if (strncmp(s + start, "null", 4) == 0 && tlen == 4) {
        if (dst && dst_cap > 0) {
            dst[0] = '\0';
        }
    } else {
        size_t copy_n = tlen < max_w ? tlen : max_w;
        if (dst && copy_n > 0) {
            memcpy(dst, s + start, copy_n);
            w = copy_n;
        }
        if (dst && dst_cap > 0) {
            dst[w] = '\0';
        }
    }

    *io_pos = pos;
    return 0;
}

size_t cjang_parse_flat_json(
    const char *body,
    size_t body_len,
    cjang_post_kv_store_t *store
) {
    if (!store) {
        return 0;
    }
    store->count = 0;
    if (!body || body_len == 0) {
        return 0;
    }

    size_t pos = skip_ws(body, 0, body_len);
    if (pos >= body_len || body[pos] != '{') {
        return 0;
    }
    pos = skip_ws(body, pos + 1, body_len);

    if (pos < body_len && body[pos] == '}') {
        return 0;
    }

    while (pos < body_len && store->count < CJANG_POST_MAX_KV) {
        pos = skip_ws(body, pos, body_len);
        cjang_post_slot_t *slot = &store->slots[store->count];
        slot->key[0] = '\0';
        slot->val[0] = '\0';

        if (parse_json_str(body, body_len, &pos, slot->key, sizeof(slot->key)) != 0) {
            break;
        }

        pos = skip_ws(body, pos, body_len);
        if (pos >= body_len || body[pos] != ':') {
            break;
        }
        pos++;

        if (parse_json_scalar(body, body_len, &pos, slot->val, sizeof(slot->val)) != 0) {
            break;
        }

        if (slot->key[0] != '\0') {
            store->count++;
        }

        pos = skip_ws(body, pos, body_len);
        if (pos < body_len && body[pos] == ',') {
            pos++;
            continue;
        }
        if (pos < body_len && body[pos] == '}') {
            break;
        }
        break;
    }

    return store->count;
}

size_t cjang_api_handle_post_auto(
    const char *tpl,
    const char *body,
    size_t body_len,
    char *http_out,
    size_t http_cap
) {
    if (!http_out || http_cap == 0) {
        return 0;
    }
    if (!tpl) {
        return cjang_api_build_http_response(
            http_out,
            http_cap,
            400,
            "Bad Request",
            "text/plain; charset=utf-8",
            "missing template"
        );
    }

    size_t first = skip_ws(body ? body : "", 0, body ? body_len : 0);
    if (body && first < body_len && body[first] == '{') {
        cjang_post_kv_store_t store;
        cjang_parse_flat_json(body, body_len, &store);

        cjang_api_ctx_t ctx;
        cjang_post_store_to_ctx(&store, &ctx);
        return cjang_api_handle_render_request(tpl, &ctx, http_out, http_cap);
    }

    return cjang_api_handle_post_render(tpl, body, body_len, http_out, http_cap);
}
