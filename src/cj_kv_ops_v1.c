#include "cj_kv_ops_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_str_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    size_t len = strlen(src);
    if (len >= cap) len = cap - 1u;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

static void append_str_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u || !src) return;
    size_t cur = strlen(dst);
    if (cur >= cap - 1u) {
        dst[cap - 1u] = '\0';
        return;
    }
    size_t avail = (cap - 1u) - cur;
    size_t slen = strlen(src);
    size_t add = (slen < avail) ? slen : avail;
    memcpy(dst + cur, src, add);
    dst[cur + add] = '\0';
}

static int starts_with_s(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    size_t plen = strlen(prefix);
    if (strlen(s) < plen) return 0;
    return strncmp(s, prefix, plen) == 0 ? 1 : 0;
}

static void escape_json_str(const char *src, char *dst, size_t cap) {
    if (!dst || cap == 0u) return;
    dst[0] = '\0';
    if (!src) return;
    size_t pos = 0u;
    for (size_t i = 0u; src[i] != '\0' && pos + 2u < cap; i++) {
        char c = src[i];
        if (c == '"' || c == '\\') {
            if (pos + 3u >= cap) break;
            dst[pos++] = '\\';
            dst[pos++] = c;
        } else if (c == '\n') {
            if (pos + 3u >= cap) break;
            dst[pos++] = '\\';
            dst[pos++] = 'n';
        } else if (c == '\r') {
            if (pos + 3u >= cap) break;
            dst[pos++] = '\\';
            dst[pos++] = 'r';
        } else {
            dst[pos++] = c;
        }
    }
    dst[pos] = '\0';
}

void cj_kv_init(cj_kv_map_t *m) {
    if (!m) return;
    memset(m, 0, sizeof(*m));
    m->count = 0u;
}

void cj_kv_clear(cj_kv_map_t *m) {
    cj_kv_init(m);
}

size_t cj_kv_count(const cj_kv_map_t *m) {
    return m ? m->count : 0u;
}

size_t cj_kv_capacity(void) {
    return CJ_KV_MAX_ENTRIES;
}

size_t cj_kv_remaining(const cj_kv_map_t *m) {
    if (!m || m->count >= CJ_KV_MAX_ENTRIES) return 0u;
    return CJ_KV_MAX_ENTRIES - m->count;
}

int cj_kv_is_empty(const cj_kv_map_t *m) {
    return (!m || m->count == 0u) ? 1 : 0;
}

int cj_kv_is_full(const cj_kv_map_t *m) {
    return (!m || m->count >= CJ_KV_MAX_ENTRIES) ? 1 : 0;
}

int cj_kv_is_valid_idx(const cj_kv_map_t *m, size_t idx) {
    return (m && idx < m->count) ? 1 : 0;
}

int cj_kv_equals(const cj_kv_map_t *a, const cj_kv_map_t *b) {
    if (!a || !b || a->count != b->count) return 0;
    for (size_t i = 0u; i < a->count; i++) {
        const char *bv = cj_kv_get_ptr(b, a->entries[i].key);
        if (!bv || strcmp(a->entries[i].val, bv) != 0) return 0;
    }
    return 1;
}

size_t cj_kv_copy(cj_kv_map_t *dst, const cj_kv_map_t *src) {
    if (!dst || !src) return 0u;
    cj_kv_init(dst);
    size_t n = (src->count < CJ_KV_MAX_ENTRIES) ? src->count : CJ_KV_MAX_ENTRIES;
    for (size_t i = 0u; i < n; i++) {
        dst->entries[i] = src->entries[i];
    }
    dst->count = n;
    return n;
}

int cj_kv_index_of(const cj_kv_map_t *m, const char *key) {
    if (!m || !key) return -1;
    for (size_t i = 0u; i < m->count; i++) {
        if (strcmp(m->entries[i].key, key) == 0) return (int)i;
    }
    return -1;
}

int cj_kv_set(cj_kv_map_t *m, const char *key, const char *val) {
    if (!m || !key || key[0] == '\0') return -1;
    int idx = cj_kv_index_of(m, key);
    if (idx >= 0) {
        copy_str_b(m->entries[(size_t)idx].val, CJ_KV_VAL_MAX, val ? val : "");
        return 0;
    }
    if (m->count >= CJ_KV_MAX_ENTRIES) return -2;
    size_t pos = m->count++;
    copy_str_b(m->entries[pos].key, CJ_KV_KEY_MAX, key);
    copy_str_b(m->entries[pos].val, CJ_KV_VAL_MAX, val ? val : "");
    return 0;
}

int cj_kv_insert_only(cj_kv_map_t *m, const char *key, const char *val) {
    if (!m || !key || key[0] == '\0') return -1;
    if (cj_kv_index_of(m, key) >= 0) return -1;
    return cj_kv_set(m, key, val);
}

int cj_kv_update_only(cj_kv_map_t *m, const char *key, const char *val) {
    if (!m || !key || key[0] == '\0') return -1;
    int idx = cj_kv_index_of(m, key);
    if (idx < 0) return -1;
    copy_str_b(m->entries[(size_t)idx].val, CJ_KV_VAL_MAX, val ? val : "");
    return 0;
}

int cj_kv_append_val(cj_kv_map_t *m, const char *key, const char *suffix) {
    if (!m || !key || key[0] == '\0') return -1;
    int idx = cj_kv_index_of(m, key);
    if (idx < 0) return -1;
    append_str_b(m->entries[(size_t)idx].val, CJ_KV_VAL_MAX, suffix ? suffix : "");
    return 0;
}

const char *cj_kv_get_ptr(const cj_kv_map_t *m, const char *key) {
    int idx = cj_kv_index_of(m, key);
    if (idx < 0) return NULL;
    return m->entries[(size_t)idx].val;
}

size_t cj_kv_get_b(const cj_kv_map_t *m, const char *key, char *dst, size_t cap, const char *fallback) {
    if (!dst || cap == 0u) return 0u;
    const char *v = cj_kv_get_ptr(m, key);
    const char *pick = v ? v : (fallback ? fallback : "");
    copy_str_b(dst, cap, pick);
    return strlen(dst);
}

int cj_kv_has_key(const cj_kv_map_t *m, const char *key) {
    return (cj_kv_index_of(m, key) >= 0) ? 1 : 0;
}

int cj_kv_has_val(const cj_kv_map_t *m, const char *val) {
    if (!m || !val) return 0;
    for (size_t i = 0u; i < m->count; i++) {
        if (strcmp(m->entries[i].val, val) == 0) return 1;
    }
    return 0;
}

int cj_kv_remove(cj_kv_map_t *m, const char *key) {
    int idx = cj_kv_index_of(m, key);
    if (idx < 0) return -1;
    size_t pos = (size_t)idx;
    for (size_t i = pos; i + 1u < m->count; i++) {
        m->entries[i] = m->entries[i + 1u];
    }
    memset(&m->entries[m->count - 1u], 0, sizeof(cj_kv_entry_t));
    m->count--;
    return 0;
}

size_t cj_kv_remove_prefix(cj_kv_map_t *m, const char *prefix) {
    if (!m || !prefix || prefix[0] == '\0') return 0u;
    size_t removed = 0u;
    size_t i = 0u;
    while (i < m->count) {
        if (starts_with_s(m->entries[i].key, prefix)) {
            (void)cj_kv_remove(m, m->entries[i].key);
            removed++;
        } else {
            i++;
        }
    }
    return removed;
}

int cj_kv_get_at(const cj_kv_map_t *m, size_t idx, char *k_dst, size_t k_cap, char *v_dst, size_t v_cap) {
    if (!cj_kv_is_valid_idx(m, idx)) return -1;
    copy_str_b(k_dst, k_cap, m->entries[idx].key);
    copy_str_b(v_dst, v_cap, m->entries[idx].val);
    return 0;
}

const char *cj_kv_key_at(const cj_kv_map_t *m, size_t idx) {
    if (!cj_kv_is_valid_idx(m, idx)) return NULL;
    return m->entries[idx].key;
}

const char *cj_kv_val_at(const cj_kv_map_t *m, size_t idx) {
    if (!cj_kv_is_valid_idx(m, idx)) return NULL;
    return m->entries[idx].val;
}

int cj_kv_set_at(cj_kv_map_t *m, size_t idx, const char *val) {
    if (!cj_kv_is_valid_idx(m, idx)) return -1;
    copy_str_b(m->entries[idx].val, CJ_KV_VAL_MAX, val ? val : "");
    return 0;
}

int cj_kv_swap_entries(cj_kv_map_t *m, size_t i, size_t j) {
    if (!cj_kv_is_valid_idx(m, i) || !cj_kv_is_valid_idx(m, j)) return -1;
    if (i == j) return 0;
    cj_kv_entry_t tmp = m->entries[i];
    m->entries[i] = m->entries[j];
    m->entries[j] = tmp;
    return 0;
}

int cj_kv_rename_key(cj_kv_map_t *m, const char *old_key, const char *new_key) {
    if (!m || !old_key || !new_key || new_key[0] == '\0') return -1;
    int idx = cj_kv_index_of(m, old_key);
    if (idx < 0) return -1;
    if (strcmp(old_key, new_key) != 0 && cj_kv_index_of(m, new_key) >= 0) return -2;
    copy_str_b(m->entries[(size_t)idx].key, CJ_KV_KEY_MAX, new_key);
    return 0;
}

size_t cj_kv_count_key_prefix(const cj_kv_map_t *m, const char *prefix) {
    if (!m || !prefix) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; i < m->count; i++) {
        if (starts_with_s(m->entries[i].key, prefix)) cnt++;
    }
    return cnt;
}

size_t cj_kv_count_val_prefix(const cj_kv_map_t *m, const char *prefix) {
    if (!m || !prefix) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; i < m->count; i++) {
        if (starts_with_s(m->entries[i].val, prefix)) cnt++;
    }
    return cnt;
}

int cj_kv_find_key_by_val(const cj_kv_map_t *m, const char *val, char *k_dst, size_t k_cap) {
    if (!m || !val) return -1;
    for (size_t i = 0u; i < m->count; i++) {
        if (strcmp(m->entries[i].val, val) == 0) {
            copy_str_b(k_dst, k_cap, m->entries[i].key);
            return 0;
        }
    }
    return -1;
}

size_t cj_kv_merge(cj_kv_map_t *dst, const cj_kv_map_t *src, int overwrite) {
    if (!dst || !src) return 0u;
    size_t touched = 0u;
    for (size_t i = 0u; i < src->count; i++) {
        int exists = cj_kv_has_key(dst, src->entries[i].key);
        if (!exists) {
            if (cj_kv_set(dst, src->entries[i].key, src->entries[i].val) == 0) touched++;
        } else if (overwrite) {
            if (cj_kv_set(dst, src->entries[i].key, src->entries[i].val) == 0) touched++;
        }
    }
    return touched;
}

size_t cj_kv_filter_key_prefix(cj_kv_map_t *dst, const cj_kv_map_t *src, const char *prefix) {
    if (!dst || !src) return 0u;
    cj_kv_init(dst);
    for (size_t i = 0u; i < src->count; i++) {
        if (starts_with_s(src->entries[i].key, prefix)) {
            (void)cj_kv_set(dst, src->entries[i].key, src->entries[i].val);
        }
    }
    return dst->count;
}

size_t cj_kv_exclude_key_prefix(cj_kv_map_t *dst, const cj_kv_map_t *src, const char *prefix) {
    if (!dst || !src) return 0u;
    cj_kv_init(dst);
    for (size_t i = 0u; i < src->count; i++) {
        if (!prefix || prefix[0] == '\0' || !starts_with_s(src->entries[i].key, prefix)) {
            (void)cj_kv_set(dst, src->entries[i].key, src->entries[i].val);
        }
    }
    return dst->count;
}

size_t cj_kv_intersect_keys(cj_kv_map_t *dst, const cj_kv_map_t *a, const cj_kv_map_t *b) {
    if (!dst || !a || !b) return 0u;
    cj_kv_init(dst);
    for (size_t i = 0u; i < a->count; i++) {
        if (cj_kv_has_key(b, a->entries[i].key)) {
            (void)cj_kv_set(dst, a->entries[i].key, a->entries[i].val);
        }
    }
    return dst->count;
}

size_t cj_kv_diff_keys(cj_kv_map_t *dst, const cj_kv_map_t *a, const cj_kv_map_t *b) {
    if (!dst || !a || !b) return 0u;
    cj_kv_init(dst);
    for (size_t i = 0u; i < a->count; i++) {
        if (!cj_kv_has_key(b, a->entries[i].key)) {
            (void)cj_kv_set(dst, a->entries[i].key, a->entries[i].val);
        }
    }
    return dst->count;
}

size_t cj_kv_keys_list_b(const cj_kv_map_t *m, char *dst, size_t cap, char sep) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!m) return 0u;
    char s[2] = { sep, '\0' };
    for (size_t i = 0u; i < m->count; i++) {
        if (i > 0u) append_str_b(dst, cap, s);
        append_str_b(dst, cap, m->entries[i].key);
    }
    return strlen(dst);
}

size_t cj_kv_vals_list_b(const cj_kv_map_t *m, char *dst, size_t cap, char sep) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!m) return 0u;
    char s[2] = { sep, '\0' };
    for (size_t i = 0u; i < m->count; i++) {
        if (i > 0u) append_str_b(dst, cap, s);
        append_str_b(dst, cap, m->entries[i].val);
    }
    return strlen(dst);
}

void cj_kv_sort_by_key(cj_kv_map_t *m) {
    if (!m || m->count < 2u) return;
    for (size_t i = 0u; i + 1u < m->count; i++) {
        for (size_t j = i + 1u; j < m->count; j++) {
            if (strcmp(m->entries[i].key, m->entries[j].key) > 0) {
                (void)cj_kv_swap_entries(m, i, j);
            }
        }
    }
}

void cj_kv_sort_by_val(cj_kv_map_t *m) {
    if (!m || m->count < 2u) return;
    for (size_t i = 0u; i + 1u < m->count; i++) {
        for (size_t j = i + 1u; j < m->count; j++) {
            if (strcmp(m->entries[i].val, m->entries[j].val) > 0) {
                (void)cj_kv_swap_entries(m, i, j);
            }
        }
    }
}

void cj_kv_reverse(cj_kv_map_t *m) {
    if (!m || m->count < 2u) return;
    size_t l = 0u, r = m->count - 1u;
    while (l < r) {
        (void)cj_kv_swap_entries(m, l, r);
        l++;
        r--;
    }
}

int cj_kv_set_int(cj_kv_map_t *m, const char *key, long long val) {
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%lld", val);
    if (n <= 0 || (size_t)n >= sizeof(buf)) return -1;
    return cj_kv_set(m, key, buf);
}

long long cj_kv_get_int(const cj_kv_map_t *m, const char *key, long long fallback) {
    const char *v = cj_kv_get_ptr(m, key);
    if (!v || v[0] == '\0') return fallback;
    char *endp = NULL;
    long long parsed = strtoll(v, &endp, 10);
    if (endp == v || *endp != '\0') return fallback;
    return parsed;
}

int cj_kv_inc_int(cj_kv_map_t *m, const char *key, long long delta, long long initial) {
    if (!m || !key || key[0] == '\0') return -1;
    long long cur = cj_kv_has_key(m, key) ? cj_kv_get_int(m, key, initial) : initial;
    return cj_kv_set_int(m, key, cur + delta);
}

int cj_kv_set_bool(cj_kv_map_t *m, const char *key, int flag) {
    return cj_kv_set(m, key, flag ? "true" : "false");
}

int cj_kv_get_bool(const cj_kv_map_t *m, const char *key, int fallback) {
    const char *v = cj_kv_get_ptr(m, key);
    if (!v) return fallback;
    if (strcmp(v, "1") == 0 || strcmp(v, "true") == 0 || strcmp(v, "yes") == 0 || strcmp(v, "on") == 0) return 1;
    if (strcmp(v, "0") == 0 || strcmp(v, "false") == 0 || strcmp(v, "no") == 0 || strcmp(v, "off") == 0) return 0;
    return fallback;
}

size_t cj_kv_format_pairs_b(const cj_kv_map_t *m, char *dst, size_t cap, char kv_sep, char item_sep) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!m) return 0u;
    char ks[2] = { kv_sep, '\0' };
    char is[2] = { item_sep, '\0' };
    for (size_t i = 0u; i < m->count; i++) {
        if (i > 0u) append_str_b(dst, cap, is);
        append_str_b(dst, cap, m->entries[i].key);
        append_str_b(dst, cap, ks);
        append_str_b(dst, cap, m->entries[i].val);
    }
    return strlen(dst);
}

size_t cj_kv_parse_pairs(cj_kv_map_t *m, const char *src, char kv_sep, char item_sep) {
    if (!m) return 0u;
    cj_kv_init(m);
    if (!src || src[0] == '\0') return 0u;

    size_t added = 0u;
    const char *p = src;
    while (*p != '\0') {
        const char *sep_item = strchr(p, item_sep);
        size_t item_len = sep_item ? (size_t)(sep_item - p) : strlen(p);

        char item[CJ_KV_KEY_MAX + CJ_KV_VAL_MAX + 4u];
        if (item_len >= sizeof(item)) item_len = sizeof(item) - 1u;
        memcpy(item, p, item_len);
        item[item_len] = '\0';

        char *eq = strchr(item, kv_sep);
        if (eq) {
            *eq = '\0';
            if (item[0] != '\0' && cj_kv_set(m, item, eq + 1) == 0) {
                added++;
            }
        }

        if (!sep_item) break;
        p = sep_item + 1;
    }
    return added;
}

size_t cj_kv_format_json_b(const cj_kv_map_t *m, char *dst, size_t cap) {
    if (!dst || cap < 3u) return 0u;
    dst[0] = '\0';
    if (!m) return 0u;

    append_str_b(dst, cap, "{");
    for (size_t i = 0u; i < m->count; i++) {
        char kesc[CJ_KV_KEY_MAX * 2u];
        char vesc[CJ_KV_VAL_MAX * 2u];
        escape_json_str(m->entries[i].key, kesc, sizeof(kesc));
        escape_json_str(m->entries[i].val, vesc, sizeof(vesc));

        if (i > 0u) append_str_b(dst, cap, ",");
        append_str_b(dst, cap, "\"");
        append_str_b(dst, cap, kesc);
        append_str_b(dst, cap, "\":\"");
        append_str_b(dst, cap, vesc);
        append_str_b(dst, cap, "\"");
    }
    append_str_b(dst, cap, "}");
    return strlen(dst);
}

size_t cj_kv_total_key_chars(const cj_kv_map_t *m) {
    if (!m) return 0u;
    size_t total = 0u;
    for (size_t i = 0u; i < m->count; i++) total += strlen(m->entries[i].key);
    return total;
}

size_t cj_kv_total_val_chars(const cj_kv_map_t *m) {
    if (!m) return 0u;
    size_t total = 0u;
    for (size_t i = 0u; i < m->count; i++) total += strlen(m->entries[i].val);
    return total;
}
