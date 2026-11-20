#include "cjang.h"
#include <string.h>

void cjang_ctx_init(cjang_ctx_t *ctx) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(*ctx));
}

int cjang_ctx_set(cjang_ctx_t *ctx, const char *key, const char *value) {
    if (!ctx || !key || *key == '\0' || !value) return 0;

    size_t k_len = strlen(key);
    size_t v_len = strlen(value);
    if (k_len >= CJANG_CTX_KEY_LEN || v_len >= CJANG_CTX_VAL_LEN) return 0;

    for (size_t i = 0; i < ctx->count; ++i) {
        if (strcmp(ctx->vars[i].key, key) == 0) {
            memcpy(ctx->vars[i].value, value, v_len);
            ctx->vars[i].value[v_len] = '\0';
            return 1;
        }
    }

    if (ctx->count >= CJANG_MAX_CTX_VARS) return 0;

    cjang_ctx_var_t *slot = &ctx->vars[ctx->count];
    memcpy(slot->key, key, k_len);
    slot->key[k_len] = '\0';
    memcpy(slot->value, value, v_len);
    slot->value[v_len] = '\0';
    ctx->count++;
    return 1;
}

const char *cjang_ctx_get(const cjang_ctx_t *ctx, const char *key) {
    if (!ctx || !key) return NULL;
    for (size_t i = 0; i < ctx->count; ++i) {
        if (strcmp(ctx->vars[i].key, key) == 0) {
            return ctx->vars[i].value;
        }
    }
    return NULL;
}

static size_t append_str(char *out, size_t out_sz, size_t pos, const char *src) {
    if (!out || out_sz == 0 || !src) return pos;
    if (pos >= out_sz - 1) return pos;

    size_t avail = (out_sz - 1) - pos;
    size_t slen = strlen(src);
    size_t take = slen < avail ? slen : avail;
    memcpy(out + pos, src, take);
    pos += take;
    out[pos] = '\0';
    return pos;
}

int cjang_render_template(const char *tmpl, const cjang_ctx_t *ctx, char *out, size_t out_sz) {
    if (!out || out_sz == 0) return 0;
    out[0] = '\0';
    if (!tmpl) return 0;

    size_t pos = 0;
    const char *p = tmpl;

    while (*p) {
        if (p[0] == '{' && p[1] == '{') {
            const char *close = strstr(p + 2, "}}");
            if (close) {
                size_t k_len = (size_t)(close - (p + 2));
                if (k_len > 0 && k_len < CJANG_CTX_KEY_LEN) {
                    char key_buf[CJANG_CTX_KEY_LEN];
                    memcpy(key_buf, p + 2, k_len);
                    key_buf[k_len] = '\0';

                    const char *val = cjang_ctx_get(ctx, key_buf);
                    if (val) {
                        pos = append_str(out, out_sz, pos, val);
                    } else {
                        pos = append_str(out, out_sz, pos, "{{");
                        pos = append_str(out, out_sz, pos, key_buf);
                        pos = append_str(out, out_sz, pos, "}}");
                    }
                    p = close + 2;
                    continue;
                }
            }
        }

        if (pos < out_sz - 1) {
            out[pos++] = *p;
            out[pos] = '\0';
        } else {
            break;
        }
        p++;
    }

    return 1;
}
