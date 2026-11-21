#include "cj_arr_ops_v1.h"

static size_t bounded_cap(size_t cap) {
    return (cap > CJ_ARR_MAX_ITEMS) ? CJ_ARR_MAX_ITEMS : cap;
}

int cj_arr_is_valid_idx(size_t len, size_t idx) {
    return (idx < len) ? 1 : 0;
}

long long cj_arr_get_safe(const long long *arr, size_t len, size_t idx, long long fallback) {
    if (!arr || idx >= len) return fallback;
    return arr[idx];
}

long long cj_arr_get_wrap(const long long *arr, size_t len, long long idx, long long fallback) {
    if (!arr || len == 0u) return fallback;
    long long l = (long long)len;
    long long r = idx % l;
    if (r < 0) r += l;
    return arr[(size_t)r];
}

int cj_arr_set_safe(long long *arr, size_t len, size_t idx, long long val) {
    if (!arr || idx >= len) return -1;
    arr[idx] = val;
    return 0;
}

int cj_arr_swap(long long *arr, size_t len, size_t i, size_t j) {
    if (!arr || i >= len || j >= len) return -1;
    if (i == j) return 0;
    long long tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    return 0;
}

void cj_arr_fill(long long *arr, size_t len, long long val) {
    if (!arr) return;
    for (size_t i = 0u; i < len; i++) arr[i] = val;
}

void cj_arr_fill_range(long long *arr, size_t len, size_t start, size_t end, long long val) {
    if (!arr || start >= end || start >= len) return;
    if (end > len) end = len;
    for (size_t i = start; i < end; i++) arr[i] = val;
}

void cj_arr_iota(long long *arr, size_t len, long long start, long long step) {
    if (!arr) return;
    long long cur = start;
    for (size_t i = 0u; i < len; i++) {
        arr[i] = cur;
        cur += step;
    }
}

int cj_arr_equals(const long long *a, size_t len_a, const long long *b, size_t len_b) {
    if (len_a != len_b) return 0;
    if (len_a == 0u) return 1;
    if (!a || !b) return 0;
    for (size_t i = 0u; i < len_a; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int cj_arr_is_empty(const long long *arr, size_t len) {
    return (!arr || len == 0u) ? 1 : 0;
}

int cj_arr_contains(const long long *arr, size_t len, long long target) {
    return (cj_arr_find_first(arr, len, target) >= 0) ? 1 : 0;
}

long long cj_arr_find_first(const long long *arr, size_t len, long long target) {
    if (!arr) return -1;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] == target) return (long long)i;
    }
    return -1;
}

long long cj_arr_find_last(const long long *arr, size_t len, long long target) {
    if (!arr || len == 0u) return -1;
    for (size_t i = len; i > 0u; i--) {
        if (arr[i - 1u] == target) return (long long)(i - 1u);
    }
    return -1;
}

size_t cj_arr_count_val(const long long *arr, size_t len, long long target) {
    if (!arr) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] == target) cnt++;
    }
    return cnt;
}

size_t cj_arr_count_gt(const long long *arr, size_t len, long long thresh) {
    if (!arr) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] > thresh) cnt++;
    }
    return cnt;
}

size_t cj_arr_count_lt(const long long *arr, size_t len, long long thresh) {
    if (!arr) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] < thresh) cnt++;
    }
    return cnt;
}

size_t cj_arr_count_between(const long long *arr, size_t len, long long lo, long long hi) {
    if (!arr) return 0u;
    if (lo > hi) {
        long long t = lo;
        lo = hi;
        hi = t;
    }
    size_t cnt = 0u;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] >= lo && arr[i] <= hi) cnt++;
    }
    return cnt;
}

int cj_arr_all_gt(const long long *arr, size_t len, long long thresh) {
    if (!arr || len == 0u) return 0;
    for (size_t i = 0u; i < len; i++) {
        if (arr[i] <= thresh) return 0;
    }
    return 1;
}

int cj_arr_any_gt(const long long *arr, size_t len, long long thresh) {
    return (cj_arr_count_gt(arr, len, thresh) > 0u) ? 1 : 0;
}

int cj_arr_none_gt(const long long *arr, size_t len, long long thresh) {
    return (cj_arr_count_gt(arr, len, thresh) == 0u) ? 1 : 0;
}

size_t cj_arr_copy_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t n = (src_len < lim) ? src_len : lim;
    for (size_t i = 0u; i < n; i++) dst[i] = src[i];
    return n;
}

size_t cj_arr_slice_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t start, size_t end) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || start >= end || start >= src_len) return 0u;
    if (end > src_len) end = src_len;
    size_t n = end - start;
    if (n > lim) n = lim;
    for (size_t i = 0u; i < n; i++) dst[i] = src[start + i];
    return n;
}

size_t cj_arr_concat_b(long long *dst, size_t dst_cap, const long long *a, size_t len_a, const long long *b, size_t len_b) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u) return 0u;
    size_t n1 = cj_arr_copy_b(dst, lim, a, len_a);
    size_t n2 = cj_arr_copy_b(dst + n1, lim - n1, b, len_b);
    return n1 + n2;
}

size_t cj_arr_reverse_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t n = (src_len < lim) ? src_len : lim;
    for (size_t i = 0u; i < n; i++) dst[i] = src[src_len - 1u - i];
    return n;
}

void cj_arr_reverse_inplace(long long *arr, size_t len) {
    if (!arr || len < 2u) return;
    size_t l = 0u, r = len - 1u;
    while (l < r) {
        (void)cj_arr_swap(arr, len, l, r);
        l++;
        r--;
    }
}

size_t cj_arr_rotate_left_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t shift) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t n = (src_len < lim) ? src_len : lim;
    size_t s = shift % src_len;
    for (size_t i = 0u; i < n; i++) {
        dst[i] = src[(i + s) % src_len];
    }
    return n;
}

size_t cj_arr_rotate_right_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t shift) {
    if (!src || src_len == 0u) return 0u;
    size_t s = shift % src_len;
    size_t left_s = (src_len - s) % src_len;
    return cj_arr_rotate_left_b(dst, dst_cap, src, src_len, left_s);
}

size_t cj_arr_dedup_sorted_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    dst[out++] = src[0];
    for (size_t i = 1u; i < src_len && out < lim; i++) {
        if (src[i] != dst[out - 1u]) {
            dst[out++] = src[i];
        }
    }
    return out;
}

size_t cj_arr_unique_preserve_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (!cj_arr_contains(dst, out, src[i])) {
            dst[out++] = src[i];
        }
    }
    return out;
}

size_t cj_arr_remove_at_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t idx) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (i != idx) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_filter_gt_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long thresh) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (src[i] > thresh) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_filter_lt_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long thresh) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (src[i] < thresh) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_filter_even_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if ((src[i] & 1LL) == 0) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_filter_odd_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if ((src[i] & 1LL) != 0) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_filter_range_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long lo, long long hi) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    if (lo > hi) {
        long long t = lo;
        lo = hi;
        hi = t;
    }
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (src[i] >= lo && src[i] <= hi) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_exclude_val_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long banned) {
    size_t lim = bounded_cap(dst_cap);
    if (!dst || lim == 0u || !src || src_len == 0u) return 0u;
    size_t out = 0u;
    for (size_t i = 0u; i < src_len && out < lim; i++) {
        if (src[i] != banned) dst[out++] = src[i];
    }
    return out;
}

size_t cj_arr_clamp_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long lo, long long hi) {
    size_t n = cj_arr_copy_b(dst, dst_cap, src, src_len);
    if (lo > hi) {
        long long t = lo;
        lo = hi;
        hi = t;
    }
    for (size_t i = 0u; i < n; i++) {
        if (dst[i] < lo) dst[i] = lo;
        else if (dst[i] > hi) dst[i] = hi;
    }
    return n;
}

size_t cj_arr_scale_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long factor) {
    size_t n = cj_arr_copy_b(dst, dst_cap, src, src_len);
    for (size_t i = 0u; i < n; i++) dst[i] *= factor;
    return n;
}

size_t cj_arr_offset_all_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, long long delta) {
    size_t n = cj_arr_copy_b(dst, dst_cap, src, src_len);
    for (size_t i = 0u; i < n; i++) dst[i] += delta;
    return n;
}

size_t cj_arr_take_head_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len, size_t n) {
    size_t want = (n < src_len) ? n : src_len;
    return cj_arr_copy_b(dst, dst_cap, src, want);
}

int cj_arr_is_sorted_asc(const long long *arr, size_t len) {
    if (!arr) return 0;
    for (size_t i = 1u; i < len; i++) {
        if (arr[i - 1u] > arr[i]) return 0;
    }
    return 1;
}

int cj_arr_is_sorted_desc(const long long *arr, size_t len) {
    if (!arr) return 0;
    for (size_t i = 1u; i < len; i++) {
        if (arr[i - 1u] < arr[i]) return 0;
    }
    return 1;
}

void cj_arr_sort_asc_inplace(long long *arr, size_t len) {
    if (!arr || len < 2u) return;
    for (size_t i = 0u; i + 1u < len; i++) {
        size_t min_idx = i;
        for (size_t j = i + 1u; j < len; j++) {
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            (void)cj_arr_swap(arr, len, i, min_idx);
        }
    }
}

void cj_arr_sort_desc_inplace(long long *arr, size_t len) {
    if (!arr || len < 2u) return;
    for (size_t i = 0u; i + 1u < len; i++) {
        size_t max_idx = i;
        for (size_t j = i + 1u; j < len; j++) {
            if (arr[j] > arr[max_idx]) max_idx = j;
        }
        if (max_idx != i) {
            (void)cj_arr_swap(arr, len, i, max_idx);
        }
    }
}

size_t cj_arr_sort_asc_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t n = cj_arr_copy_b(dst, dst_cap, src, src_len);
    cj_arr_sort_asc_inplace(dst, n);
    return n;
}

size_t cj_arr_sort_desc_b(long long *dst, size_t dst_cap, const long long *src, size_t src_len) {
    size_t n = cj_arr_copy_b(dst, dst_cap, src, src_len);
    cj_arr_sort_desc_inplace(dst, n);
    return n;
}

long long cj_arr_sum(const long long *arr, size_t len) {
    if (!arr || len == 0u) return 0;
    long long s = 0;
    for (size_t i = 0u; i < len; i++) s += arr[i];
    return s;
}

long long cj_arr_product(const long long *arr, size_t len) {
    if (!arr || len == 0u) return 0;
    long long p = 1;
    for (size_t i = 0u; i < len; i++) p *= arr[i];
    return p;
}

long long cj_arr_min(const long long *arr, size_t len, long long fallback) {
    if (!arr || len == 0u) return fallback;
    long long m = arr[0];
    for (size_t i = 1u; i < len; i++) {
        if (arr[i] < m) m = arr[i];
    }
    return m;
}

long long cj_arr_max(const long long *arr, size_t len, long long fallback) {
    if (!arr || len == 0u) return fallback;
    long long m = arr[0];
    for (size_t i = 1u; i < len; i++) {
        if (arr[i] > m) m = arr[i];
    }
    return m;
}
