#include "cj_arr_ops_v1.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    long long out[64];
    long long src[6] = {10, 4, 20, 4, 15, 8};

    assert(cj_arr_is_valid_idx(6u, 5u) == 1 && cj_arr_is_valid_idx(6u, 6u) == 0);
    assert(cj_arr_get_safe(src, 6u, 2u, -1) == 20);
    assert(cj_arr_get_safe(src, 6u, 99u, -1) == -1);
    assert(cj_arr_get_wrap(src, 6u, -1, 0) == 8);
    assert(cj_arr_is_empty(src, 0u) == 1 && cj_arr_is_empty(src, 6u) == 0);

    long long mut[4] = {1, 2, 3, 4};
    assert(cj_arr_set_safe(mut, 4u, 1u, 22) == 0 && mut[1] == 22);
    assert(cj_arr_swap(mut, 4u, 0u, 3u) == 0 && mut[0] == 4 && mut[3] == 1);

    cj_arr_fill(out, 4u, 7);
    assert(out[0] == 7 && out[3] == 7);
    cj_arr_fill_range(out, 4u, 1u, 3u, 9);
    assert(out[0] == 7 && out[1] == 9 && out[2] == 9 && out[3] == 7);

    long long seq[5];
    cj_arr_iota(seq, 5u, 2, 3);
    long long exp_seq[5] = {2, 5, 8, 11, 14};
    assert(cj_arr_equals(seq, 5u, exp_seq, 5u) == 1);

    assert(cj_arr_contains(src, 6u, 15) == 1);
    assert(cj_arr_find_first(src, 6u, 4) == 1);
    assert(cj_arr_find_last(src, 6u, 4) == 3);
    assert(cj_arr_count_val(src, 6u, 4) == 2u);
    assert(cj_arr_count_gt(src, 6u, 10) == 2u);
    assert(cj_arr_count_lt(src, 6u, 10) == 3u);
    assert(cj_arr_count_between(src, 6u, 8, 15) == 3u);
    assert(cj_arr_all_gt(src, 6u, 3) == 1);
    assert(cj_arr_any_gt(src, 6u, 19) == 1);
    assert(cj_arr_none_gt(src, 6u, 100) == 1);

    assert(cj_arr_copy_b(out, 64u, src, 6u) == 6u);
    assert(cj_arr_slice_b(out, 64u, src, 6u, 1u, 4u) == 3u && out[0] == 4 && out[2] == 4);
    assert(cj_arr_concat_b(out, 64u, mut, 2u, mut + 2, 2u) == 4u);
    assert(cj_arr_reverse_b(out, 64u, mut, 4u) == 4u && out[0] == 1 && out[3] == 4);
    cj_arr_reverse_inplace(out, 4u);
    assert(out[0] == 4 && out[3] == 1);

    assert(cj_arr_rotate_left_b(out, 64u, mut, 4u, 1u) == 4u && out[0] == 22 && out[3] == 4);
    assert(cj_arr_rotate_right_b(out, 64u, mut, 4u, 1u) == 4u && out[0] == 1);

    long long sorted_in[5] = {1, 1, 2, 3, 3};
    assert(cj_arr_dedup_sorted_b(out, 64u, sorted_in, 5u) == 3u);
    assert(cj_arr_unique_preserve_b(out, 64u, src, 6u) == 5u);
    assert(cj_arr_remove_at_b(out, 64u, mut, 4u, 1u) == 3u);

    assert(cj_arr_filter_gt_b(out, 64u, src, 6u, 10) == 2u);
    assert(cj_arr_filter_lt_b(out, 64u, src, 6u, 10) == 3u);
    assert(cj_arr_filter_even_b(out, 64u, src, 6u) == 5u);
    assert(cj_arr_filter_odd_b(out, 64u, src, 6u) == 1u);
    assert(cj_arr_filter_range_b(out, 64u, src, 6u, 8, 15) == 3u);
    assert(cj_arr_exclude_val_b(out, 64u, src, 6u, 4) == 4u);
    assert(cj_arr_clamp_all_b(out, 64u, src, 6u, 6, 15) == 6u && out[1] == 6 && out[2] == 15);
    assert(cj_arr_scale_all_b(out, 64u, mut, 4u, 2) == 4u && out[0] == 8);
    assert(cj_arr_offset_all_b(out, 64u, mut, 4u, 1) == 4u && out[0] == 5);
    assert(cj_arr_take_head_b(out, 64u, src, 6u, 2u) == 2u && out[0] == 10 && out[1] == 4);

    assert(cj_arr_is_sorted_asc(sorted_in, 5u) == 1);
    assert(cj_arr_is_sorted_desc(sorted_in, 5u) == 0);
    size_t sn = cj_arr_sort_asc_b(out, 64u, src, 6u);
    assert(sn == 6u && cj_arr_is_sorted_asc(out, sn) == 1);
    sn = cj_arr_sort_desc_b(out, 64u, src, 6u);
    assert(sn == 6u && cj_arr_is_sorted_desc(out, sn) == 1);

    long long ip[4] = {3, 1, 4, 2};
    cj_arr_sort_asc_inplace(ip, 4u);
    assert(cj_arr_is_sorted_asc(ip, 4u) == 1);
    cj_arr_sort_desc_inplace(ip, 4u);
    assert(cj_arr_is_sorted_desc(ip, 4u) == 1);

    assert(cj_arr_sum(ip, 4u) == 10);
    assert(cj_arr_product(ip, 4u) == 24);
    assert(cj_arr_min(ip, 4u, 0) == 1);
    assert(cj_arr_max(ip, 4u, 0) == 4);

    printf("OK: cj_arr_ops_v1 (50 functions) passed\n");
    return 0;
}
