#include "api_post.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ascii_strncasecmp(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca == '\0' || cb == '\0') {
            return (int)ca - (int)cb;
        }
        int da = tolower(ca);
        int db = tolower(cb);
        if (da != db) {
            return da - db;
        }
    }
    return 0;
}

int cjang_parse_content_length(const char *headers, size_t max_body_cap, size_t *out_len) {
    if (!headers || !out_len || max_body_cap == 0) {
        return -1;
    }
    *out_len = 0;

    const char *p = headers;
    const char *needle = "content-length:";
    const size_t nlen = strlen(needle);

    while (*p != '\0') {
        const char *line_end = strstr(p, "\r\n");
        size_t line_len = line_end ? (size_t)(line_end - p) : strlen(p);

        if (line_len > nlen && ascii_strncasecmp(p, needle, nlen) == 0) {
            const char *val_p = p + nlen;
            const char *end_p = p + line_len;
            while (val_p < end_p && (*val_p == ' ' || *val_p == '\t')) {
                val_p++;
            }
            if (val_p >= end_p || !isdigit((unsigned char)*val_p)) {
                return -1;
            }

            char num_buf[32];
            size_t ncopy = (size_t)(end_p - val_p);
            if (ncopy >= sizeof(num_buf)) {
                return -1;
            }
            memcpy(num_buf, val_p, ncopy);
            num_buf[ncopy] = '\0';

            errno = 0;
            char *parse_end = NULL;
            unsigned long long parsed = strtoull(num_buf, &parse_end, 10);
            if (errno != 0 || parse_end == num_buf || *parse_end != '\0') {
                return -1;
            }
            if (parsed > (unsigned long long)max_body_cap) {
                return -2;
            }
            *out_len = (size_t)parsed;
            return 0;
        }

        if (!line_end) {
            break;
        }
        p = line_end + 2;
    }

    return 0;
}

static void copy_segment(char *dst, size_t cap, const char *src, size_t len) {
    if (!dst || cap == 0) {
        return;
    }
    size_t n = len < (cap - 1) ? len : (cap - 1);
    if (src && n > 0) {
        memcpy(dst, src, n);
    }
    dst[n] = '\0';
}

size_t cjang_parse_form_pairs(
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

    size_t pos = 0;
    while (pos < body_len && store->count < CJANG_POST_MAX_KV) {
        size_t amp = pos;
        while (amp < body_len && body[amp] != '&') {
            amp++;
        }

        size_t eq = pos;
        while (eq < amp && body[eq] != '=') {
            eq++;
        }

        if (eq > pos) {
            size_t klen = eq - pos;
            size_t vlen = (eq < amp) ? (amp - eq - 1) : 0;
            cjang_post_slot_t *slot = &store->slots[store->count];
            copy_segment(slot->key, sizeof(slot->key), body + pos, klen);
            copy_segment(slot->val, sizeof(slot->val), (eq < amp) ? (body + eq + 1) : "", vlen);
            store->count++;
        }

        pos = (amp < body_len) ? (amp + 1) : body_len;
    }

    return store->count;
}

void cjang_post_store_to_ctx(const cjang_post_kv_store_t *store, cjang_api_ctx_t *out_ctx) {
    if (!out_ctx) {
        return;
    }
    if (!store || store->count == 0) {
        out_ctx->items = NULL;
        out_ctx->count = 0;
        return;
    }
    cjang_post_kv_store_t *mutable_store = (cjang_post_kv_store_t *)store;
    for (size_t i = 0; i < mutable_store->count; i++) {
        mutable_store->view[i].key = mutable_store->slots[i].key;
        mutable_store->view[i].val = mutable_store->slots[i].val;
    }
    out_ctx->items = mutable_store->view;
    out_ctx->count = mutable_store->count;
}

size_t cjang_api_handle_post_render(
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

    cjang_post_kv_store_t store;
    cjang_parse_form_pairs(body, body_len, &store);

    cjang_api_ctx_t ctx;
    cjang_post_store_to_ctx(&store, &ctx);

    return cjang_api_handle_render_request(tpl, &ctx, http_out, http_cap);
}
