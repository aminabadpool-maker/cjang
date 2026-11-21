#ifndef CJ_STR_OPS_V1_H
#define CJ_STR_OPS_V1_H

#include <stddef.h>

/* 1-10: Length, copy, append & safety */
size_t cj_str_len_s(const char *s);
int cj_str_is_empty(const char *s);
size_t cj_str_copy_b(char *dst, size_t cap, const char *src);
size_t cj_str_cat_b(char *dst, size_t cap, const char *src);
void cj_str_clear_b(char *dst, size_t cap);
int cj_str_equals(const char *a, const char *b);
int cj_str_equals_ci(const char *a, const char *b);
int cj_str_compare_ci(const char *a, const char *b);
size_t cj_str_repeat_b(char *dst, size_t cap, const char *src, size_t times);
size_t cj_str_reverse_b(char *dst, size_t cap, const char *src);

/* 11-20: Case conversion */
char cj_char_to_lower(char c);
char cj_char_to_upper(char c);
size_t cj_str_to_lower_b(char *dst, size_t cap, const char *src);
size_t cj_str_to_upper_b(char *dst, size_t cap, const char *src);
size_t cj_str_capitalize_b(char *dst, size_t cap, const char *src);
size_t cj_str_title_b(char *dst, size_t cap, const char *src);
size_t cj_str_swapcase_b(char *dst, size_t cap, const char *src);
int cj_str_is_lower(const char *s);
int cj_str_is_upper(const char *s);
size_t cj_str_slugify_b(char *dst, size_t cap, const char *src);

/* 21-30: Trim & whitespace */
int cj_char_is_space(char c);
size_t cj_str_lstrip_b(char *dst, size_t cap, const char *src);
size_t cj_str_rstrip_b(char *dst, size_t cap, const char *src);
size_t cj_str_strip_b(char *dst, size_t cap, const char *src);
size_t cj_str_collapse_ws_b(char *dst, size_t cap, const char *src);
size_t cj_str_strip_char_b(char *dst, size_t cap, const char *src, char ch);
size_t cj_str_pad_left_b(char *dst, size_t cap, const char *src, size_t width, char pad);
size_t cj_str_pad_right_b(char *dst, size_t cap, const char *src, size_t width, char pad);
size_t cj_str_center_b(char *dst, size_t cap, const char *src, size_t width, char pad);
size_t cj_str_count_ws(const char *s);

/* 31-40: Search, prefix, suffix & substring */
int cj_str_starts_with(const char *s, const char *prefix);
int cj_str_ends_with(const char *s, const char *suffix);
int cj_str_contains(const char *haystack, const char *needle);
int cj_str_find_char(const char *s, char ch);
int cj_str_rfind_char(const char *s, char ch);
int cj_str_find_sub(const char *haystack, const char *needle);
size_t cj_str_count_char(const char *s, char ch);
size_t cj_str_count_sub(const char *haystack, const char *needle);
size_t cj_str_slice_b(char *dst, size_t cap, const char *src, size_t start, size_t end);
size_t cj_str_replace_first_b(char *dst, size_t cap, const char *src, const char *old_s, const char *new_s);

/* 41-50: Character checks, lines & numbers */
int cj_char_is_digit(char c);
int cj_char_is_alpha(char c);
int cj_char_is_alnum(char c);
int cj_str_is_digits(const char *s);
int cj_str_is_alpha(const char *s);
int cj_str_is_alnum(const char *s);
size_t cj_str_count_lines(const char *s);
size_t cj_str_count_words(const char *s);
size_t cj_str_from_int_b(char *dst, size_t cap, long long val);
int cj_str_to_int_safe(const char *s, long long *out_val, long long fallback);

#endif
