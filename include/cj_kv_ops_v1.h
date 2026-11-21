#ifndef CJ_KV_OPS_V1_H
#define CJ_KV_OPS_V1_H

#include <stddef.h>

#ifndef CJ_KV_MAX_ENTRIES
#define CJ_KV_MAX_ENTRIES 128u
#endif

#ifndef CJ_KV_KEY_MAX
#define CJ_KV_KEY_MAX 64u
#endif

#ifndef CJ_KV_VAL_MAX
#define CJ_KV_VAL_MAX 192u
#endif

typedef struct {
    char key[CJ_KV_KEY_MAX];
    char val[CJ_KV_VAL_MAX];
} cj_kv_entry_t;

typedef struct {
    cj_kv_entry_t entries[CJ_KV_MAX_ENTRIES];
    size_t count;
} cj_kv_map_t;

/* 1-10: Lifecycle, state & bounds */
void cj_kv_init(cj_kv_map_t *m);
void cj_kv_clear(cj_kv_map_t *m);
size_t cj_kv_count(const cj_kv_map_t *m);
size_t cj_kv_capacity(void);
size_t cj_kv_remaining(const cj_kv_map_t *m);
int cj_kv_is_empty(const cj_kv_map_t *m);
int cj_kv_is_full(const cj_kv_map_t *m);
int cj_kv_is_valid_idx(const cj_kv_map_t *m, size_t idx);
int cj_kv_equals(const cj_kv_map_t *a, const cj_kv_map_t *b);
size_t cj_kv_copy(cj_kv_map_t *dst, const cj_kv_map_t *src);

/* 11-20: Core set, get, update & delete */
int cj_kv_set(cj_kv_map_t *m, const char *key, const char *val);
int cj_kv_insert_only(cj_kv_map_t *m, const char *key, const char *val);
int cj_kv_update_only(cj_kv_map_t *m, const char *key, const char *val);
int cj_kv_append_val(cj_kv_map_t *m, const char *key, const char *suffix);
const char *cj_kv_get_ptr(const cj_kv_map_t *m, const char *key);
size_t cj_kv_get_b(const cj_kv_map_t *m, const char *key, char *dst, size_t cap, const char *fallback);
int cj_kv_has_key(const cj_kv_map_t *m, const char *key);
int cj_kv_has_val(const cj_kv_map_t *m, const char *val);
int cj_kv_remove(cj_kv_map_t *m, const char *key);
size_t cj_kv_remove_prefix(cj_kv_map_t *m, const char *prefix);

/* 21-30: Index access, search & inspection */
int cj_kv_index_of(const cj_kv_map_t *m, const char *key);
int cj_kv_get_at(const cj_kv_map_t *m, size_t idx, char *k_dst, size_t k_cap, char *v_dst, size_t v_cap);
const char *cj_kv_key_at(const cj_kv_map_t *m, size_t idx);
const char *cj_kv_val_at(const cj_kv_map_t *m, size_t idx);
int cj_kv_set_at(cj_kv_map_t *m, size_t idx, const char *val);
int cj_kv_swap_entries(cj_kv_map_t *m, size_t i, size_t j);
int cj_kv_rename_key(cj_kv_map_t *m, const char *old_key, const char *new_key);
size_t cj_kv_count_key_prefix(const cj_kv_map_t *m, const char *prefix);
size_t cj_kv_count_val_prefix(const cj_kv_map_t *m, const char *prefix);
int cj_kv_find_key_by_val(const cj_kv_map_t *m, const char *val, char *k_dst, size_t k_cap);

/* 31-40: Bulk operations, merge, filter & diff */
size_t cj_kv_merge(cj_kv_map_t *dst, const cj_kv_map_t *src, int overwrite);
size_t cj_kv_filter_key_prefix(cj_kv_map_t *dst, const cj_kv_map_t *src, const char *prefix);
size_t cj_kv_exclude_key_prefix(cj_kv_map_t *dst, const cj_kv_map_t *src, const char *prefix);
size_t cj_kv_intersect_keys(cj_kv_map_t *dst, const cj_kv_map_t *a, const cj_kv_map_t *b);
size_t cj_kv_diff_keys(cj_kv_map_t *dst, const cj_kv_map_t *a, const cj_kv_map_t *b);
size_t cj_kv_keys_list_b(const cj_kv_map_t *m, char *dst, size_t cap, char sep);
size_t cj_kv_vals_list_b(const cj_kv_map_t *m, char *dst, size_t cap, char sep);
void cj_kv_sort_by_key(cj_kv_map_t *m);
void cj_kv_sort_by_val(cj_kv_map_t *m);
void cj_kv_reverse(cj_kv_map_t *m);

/* 41-50: Typed helpers (int/bool) & serialization */
int cj_kv_set_int(cj_kv_map_t *m, const char *key, long long val);
long long cj_kv_get_int(const cj_kv_map_t *m, const char *key, long long fallback);
int cj_kv_inc_int(cj_kv_map_t *m, const char *key, long long delta, long long initial);
int cj_kv_set_bool(cj_kv_map_t *m, const char *key, int flag);
int cj_kv_get_bool(const cj_kv_map_t *m, const char *key, int fallback);
size_t cj_kv_format_pairs_b(const cj_kv_map_t *m, char *dst, size_t cap, char kv_sep, char item_sep);
size_t cj_kv_parse_pairs(cj_kv_map_t *m, const char *src, char kv_sep, char item_sep);
size_t cj_kv_format_json_b(const cj_kv_map_t *m, char *dst, size_t cap);
size_t cj_kv_total_key_chars(const cj_kv_map_t *m);
size_t cj_kv_total_val_chars(const cj_kv_map_t *m);

#endif
