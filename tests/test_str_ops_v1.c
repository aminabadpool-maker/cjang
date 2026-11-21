#include "cj_str_ops_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[128];

    assert(cj_str_len_s("cjang") == 5u);
    assert(cj_str_is_empty("") == 1 && cj_str_is_empty("x") == 0);
    assert(cj_str_copy_b(buf, sizeof(buf), "hi") == 2u);
    assert(cj_str_cat_b(buf, sizeof(buf), " there") == 8u);
    assert(cj_str_equals(buf, "hi there") == 1);
    assert(cj_str_equals_ci("CJANG", "cjang") == 1);
    assert(cj_str_compare_ci("abc", "ABD") < 0);
    assert(cj_str_repeat_b(buf, sizeof(buf), "ab", 3u) == 6u && cj_str_equals(buf, "ababab"));
    assert(cj_str_reverse_b(buf, sizeof(buf), "abcd") == 4u && cj_str_equals(buf, "dcba"));
    cj_str_clear_b(buf, sizeof(buf));
    assert(buf[0] == '\0');

    assert(cj_str_to_lower_b(buf, sizeof(buf), "AbC") == 3u && cj_str_equals(buf, "abc"));
    assert(cj_str_to_upper_b(buf, sizeof(buf), "AbC") == 3u && cj_str_equals(buf, "ABC"));
    assert(cj_str_capitalize_b(buf, sizeof(buf), "hELLO") == 5u && cj_str_equals(buf, "Hello"));
    assert(cj_str_title_b(buf, sizeof(buf), "hello c11 world") == 15u && cj_str_equals(buf, "Hello C11 World"));
    assert(cj_str_swapcase_b(buf, sizeof(buf), "Hi") == 2u && cj_str_equals(buf, "hI"));
    assert(cj_str_is_lower("c11") == 1 && cj_str_is_upper("C11") == 1);
    assert(cj_str_slugify_b(buf, sizeof(buf), "Hello, C11 World!") == 15u && cj_str_equals(buf, "hello-c11-world"));

    assert(cj_char_is_space('\n') == 1);
    assert(cj_str_lstrip_b(buf, sizeof(buf), "  hi") == 2u && cj_str_equals(buf, "hi"));
    assert(cj_str_rstrip_b(buf, sizeof(buf), "hi  ") == 2u && cj_str_equals(buf, "hi"));
    assert(cj_str_strip_b(buf, sizeof(buf), "  hi  ") == 2u && cj_str_equals(buf, "hi"));
    assert(cj_str_collapse_ws_b(buf, sizeof(buf), "  a   b\tc  ") == 5u && cj_str_equals(buf, "a b c"));
    assert(cj_str_strip_char_b(buf, sizeof(buf), "1-2-3", '-') == 3u && cj_str_equals(buf, "123"));
    assert(cj_str_pad_left_b(buf, sizeof(buf), "7", 3u, '0') == 3u && cj_str_equals(buf, "007"));
    assert(cj_str_pad_right_b(buf, sizeof(buf), "7", 3u, '.') == 3u && cj_str_equals(buf, "7.."));
    assert(cj_str_center_b(buf, sizeof(buf), "hi", 4u, '-') == 4u);
    assert(cj_str_count_ws("a b c") == 2u);

    assert(cj_str_starts_with("/note/thread", "/note") == 1);
    assert(cj_str_ends_with("main.c", ".c") == 1);
    assert(cj_str_contains("hello cjang", "jang") == 1);
    assert(cj_str_find_char("abcdef", 'd') == 3);
    assert(cj_str_rfind_char("ababa", 'a') == 4);
    assert(cj_str_find_sub("hello world", "world") == 6);
    assert(cj_str_count_char("banana", 'a') == 3u);
    assert(cj_str_count_sub("ababab", "ab") == 3u);
    assert(cj_str_slice_b(buf, sizeof(buf), "abcdef", 1u, 4u) == 3u && cj_str_equals(buf, "bcd"));
    assert(cj_str_replace_first_b(buf, sizeof(buf), "foo bar foo", "foo", "baz") == 11u && cj_str_equals(buf, "baz bar foo"));

    assert(cj_char_is_digit('9') == 1 && cj_char_is_alpha('z') == 1 && cj_char_is_alnum('5') == 1);
    assert(cj_str_is_digits("12345") == 1 && cj_str_is_alpha("abc") == 1 && cj_str_is_alnum("a1") == 1);
    assert(cj_str_count_lines("a\nb\nc") == 3u);
    assert(cj_str_count_words("one two three") == 3u);
    assert(cj_str_from_int_b(buf, sizeof(buf), -42) == 3u && cj_str_equals(buf, "-42"));

    long long parsed = 0;
    assert(cj_str_to_int_safe("-123", &parsed, 0) == 0 && parsed == -123);
    assert(cj_str_to_int_safe("abc", &parsed, 99) == -1 && parsed == 99);

    printf("OK: cj_str_ops_v1 (50 functions) passed\n");
    return 0;
}
