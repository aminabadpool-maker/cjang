#ifndef CJ_ARR_OPS_V1_H
#define CJ_ARR_OPS_V1_H

#include <stddef.h>

#ifndef CJ_ARR_MAX_ITEMS
#define CJ_ARR_MAX_ITEMS 512u
#endif

/* 1-10: Bounds, get/set & predicates */
int cj_arr_is_valid_idx(size_t len, size_t idx);
long long cj_arr_get_safe(const long long *arr, size_t len, size_t idx, long long fallback);
long long cj_arr_get_wrap(const long long *arr, size_t len, long long idx, long long fallback);
int cj_arr_set_safe(long long *arr, size_t len, size_t idx, long long val);
int cj_arr_swap(long long *arr, size_t len, size_t i, size_t j);
void cj_arr_fill(long long *arr, size_t len, long long val);
void cj_arr_fill_range(long long *arr, size_t len, size_t start, size_t end, long long val);
void cj_arr_iota(long long *arr, size_t len, long long start, long long step);
int cj_arr_equals(const long long *a, size_t len_a, const long long *b, size_t len_b);
int cj_arr_is_empty(const long long *arr, size_t len);

/* 11-20: Search, membership & counting */
int cj_arr_contains(const long long *arr, size_t len, long long target);
long long cj_arr_find_first(const long long *arr, size_t len, long long target);
long long cj_arr_find_last(const long long *arr, size_t len, long long target);
size_t cj_arr_count_val(const long long *arr, size_t len, long long target);
size_t cj_arr_count_gt(const long long *arr, size_t len, long long thresh);
size_t cj_arr_count_lt(const long long *arr, size_t len, long long thresh);
size_t cj_arr_count_between(const long long *arr, size_t len, long long lo, long long hi);
int cj_arr_all_gt(const long long *arr, size_t len, long long thresh);
int cj_arr_any_gt(const long long *arr, size_t len, long long thresh);
int cj_arr_none_gt(const long long *arr, size_t len, long long thresh);

/* 21-30: Copy, slice, concat & manipulation */
size_t cj_arr_copy_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_slice_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t start, size_t end);
size_t cj_arr_concat_b(long long *dst, size_t dst_cap, const long long *a, size_t len_a, const long long *b, size_t len_b);
size_t cj_arr_reverse_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
void cj_arr_reverse_inplace(long long *arr, size_t len);
size_t cj_arr_rotate_left_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t shift);
size_t cj_arr_rotate_right_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t shift);
size_t cj_arr_dedup_sorted_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_unique_preserve_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_remove_at_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t idx);

/* 31-40: Filtering & partitioning */
size_t cj_arr_filter_gt_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long thresh);
size_t cj_arr_filter_lt_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long thresh);
size_t cj_arr_filter_even_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_filter_odd_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_filter_range_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long lo, long long hi);
size_t cj_arr_exclude_val_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long banned);
size_t cj_arr_clamp_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long lo, long long hi);
size_t cj_arr_scale_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long factor);
size_t cj_arr_offset_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long delta);
size_t cj_arr_take_head_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t n);

/* 41-50: Sorting, order checks & reductions */
int cj_arr_is_sorted_asc(const long long *arr, size_t len);
int cj_arr_is_sorted_desc(const long long *arr, size_t len);
size_t cj_arr_sort_asc_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
size_t cj_arr_sort_desc_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len);
void cj_arr_sort_asc_inplace(long long *arr, size_t len);
void cj_arr_sort_desc_inplace(long long *arr, size_t len);
long long cj_arr_sum(const long long *arr, size_t len);
long long cj_arr_product(const long long *arr, size_t len);
long long cj_arr_min(const long long *arr, size_t len, long long fallback);
long long cj_arr_max(const long long *arr, size_t len, long long fallback);

#endif
