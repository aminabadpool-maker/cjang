#include "cj_kv_ops_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_kv_map_t m, m2, out;
    char buf[512];
    char kbuf[64];
    char vbuf[192];

    cj_kv_init(&m);
    assert(cj_kv_is_empty(&m) == 1 && cj_kv_is_full(&m) == 0);
    assert(cj_kv_capacity() == CJ_KV_MAX_ENTRIES);
    assert(cj_kv_remaining(&m) == CJ_KV_MAX_ENTRIES);
    assert(cj_kv_is_valid_idx(&m, 0u) == 0);

    assert(cj_kv_set(&m, "app_name", "cjang") == 0);
    assert(cj_kv_set(&m, "app_mode", "dev") == 0);
    assert(cj_kv_set(&m, "lang", "c11") == 0);
    assert(cj_kv_count(&m) == 3u && cj_kv_is_empty(&m) == 0);
    assert(cj_kv_is_valid_idx(&m, 2u) == 1);

    assert(cj_kv_insert_only(&m, "app_name", "other") != 0);
    assert(cj_kv_insert_only(&m, "env", "termux") == 0);
    assert(cj_kv_update_only(&m, "missing", "x") != 0);
    assert(cj_kv_update_only(&m, "app_mode", "prod") == 0);
    assert(cj_kv_append_val(&m, "app_mode", "_v58") == 0);
    assert(strcmp(cj_kv_get_ptr(&m, "app_mode"), "prod_v58") == 0);
    assert(cj_kv_get_b(&m, "nope", buf, sizeof(buf), "def") == 3u && strcmp(buf, "def") == 0);
    assert(cj_kv_has_key(&m, "lang") == 1 && cj_kv_has_val(&m, "c11") == 1);

    assert(cj_kv_index_of(&m, "lang") == 2);
    assert(cj_kv_get_at(&m, 0u, kbuf, sizeof(kbuf), vbuf, sizeof(vbuf)) == 0);
    assert(strcmp(kbuf, "app_name") == 0 && strcmp(vbuf, "cjang") == 0);
    assert(strcmp(cj_kv_key_at(&m, 0u), "app_name") == 0);
    assert(strcmp(cj_kv_val_at(&m, 0u), "cjang") == 0);
    assert(cj_kv_set_at(&m, 2u, "c11_std") == 0 && strcmp(cj_kv_val_at(&m, 2u), "c11_std") == 0);
    assert(cj_kv_swap_entries(&m, 0u, 1u) == 0);
    assert(cj_kv_rename_key(&m, "env", "runtime_env") == 0 && cj_kv_has_key(&m, "runtime_env") == 1);
    assert(cj_kv_count_key_prefix(&m, "app_") == 2u);
    assert(cj_kv_count_val_prefix(&m, "prod") == 1u);
    assert(cj_kv_find_key_by_val(&m, "cjang", kbuf, sizeof(kbuf)) == 0 && strcmp(kbuf, "app_name") == 0);

    cj_kv_copy(&m2, &m);
    assert(cj_kv_equals(&m, &m2) == 1);
    assert(cj_kv_merge(&m2, &m, 0) == 0u);
    assert(cj_kv_filter_key_prefix(&out, &m, "app_") == 2u);
    assert(cj_kv_exclude_key_prefix(&out, &m, "app_") == 2u);
    assert(cj_kv_intersect_keys(&out, &m, &m2) == 4u);
    assert(cj_kv_diff_keys(&out, &m, &m2) == 0u);

    assert(cj_kv_keys_list_b(&m, buf, sizeof(buf), ',') > 0u);
    assert(cj_kv_vals_list_b(&m, buf, sizeof(buf), ',') > 0u);
    cj_kv_sort_by_key(&m2);
    cj_kv_sort_by_val(&m2);
    cj_kv_reverse(&m2);

    assert(cj_kv_set_int(&m, "port", 8080) == 0);
    assert(cj_kv_get_int(&m, "port", 0) == 8080);
    assert(cj_kv_inc_int(&m, "port", 5, 8080) == 0 && cj_kv_get_int(&m, "port", 0) == 8085);
    assert(cj_kv_set_bool(&m, "debug", 1) == 0 && cj_kv_get_bool(&m, "debug", 0) == 1);

    assert(cj_kv_format_pairs_b(&m, buf, sizeof(buf), '=', ';') > 0u);
    assert(cj_kv_parse_pairs(&out, "a=1;b=2", '=', ';') == 2u && cj_kv_get_int(&out, "b", 0) == 2);
    assert(cj_kv_format_json_b(&out, buf, sizeof(buf)) > 0u && strstr(buf, "\"a\":\"1\"") != NULL);
    assert(cj_kv_total_key_chars(&out) == 2u && cj_kv_total_val_chars(&out) == 2u);

    assert(cj_kv_remove_prefix(&m, "app_") == 2u);
    assert(cj_kv_remove(&m, "port") == 0);
    cj_kv_clear(&m);
    assert(cj_kv_is_empty(&m) == 1);

    printf("OK: cj_kv_ops_v1 (50 functions) passed\n");
    return 0;
}
