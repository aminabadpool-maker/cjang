#include "cj_str_ops_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t cj_str_len_s(const char *s) {
    return s ? strlen(s) : 0u;
}

int cj_str_is_empty(const char *s) {
    return (!s || s[0] == '\0') ? 1 : 0;
}

size_t cj_str_copy_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    if (!src) {
        dst[0] = '\0';
        return 0u;
    }
    size_t len = strlen(src);
    if (len >= cap) len = cap - 1u;
    memcpy(dst, src, len);
    dst[len] = '\0';
    return len;
}

size_t cj_str_cat_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    size_t cur = strlen(dst);
    if (cur >= cap - 1u) {
        dst[cap - 1u] = '\0';
        return cur;
    }
    if (!src) return cur;
    size_t avail = (cap - 1u) - cur;
    size_t slen = strlen(src);
    size_t add = (slen < avail) ? slen : avail;
    memcpy(dst + cur, src, add);
    dst[cur + add] = '\0';
    return cur + add;
}

void cj_str_clear_b(char *dst, size_t cap) {
    if (!dst || cap == 0u) return;
    memset(dst, 0, cap);
}

int cj_str_equals(const char *a, const char *b) {
    if (!a || !b) return 0;
    return strcmp(a, b) == 0 ? 1 : 0;
}

char cj_char_to_lower(char c) {
    if (c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

char cj_char_to_upper(char c) {
    if (c >= 'a' && c <= 'z') return (char)(c - ('a' - 'A'));
    return c;
}

int cj_str_equals_ci(const char *a, const char *b) {
    if (!a || !b) return 0;
    size_t i = 0u;
    while (a[i] != '\0' && b[i] != '\0') {
        if (cj_char_to_lower(a[i]) != cj_char_to_lower(b[i])) return 0;
        i++;
    }
    return (a[i] == '\0' && b[i] == '\0') ? 1 : 0;
}

int cj_str_compare_ci(const char *a, const char *b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    size_t i = 0u;
    while (a[i] != '\0' && b[i] != '\0') {
        unsigned char ca = (unsigned char)cj_char_to_lower(a[i]);
        unsigned char cb = (unsigned char)cj_char_to_lower(b[i]);
        if (ca != cb) return (ca < cb) ? -1 : 1;
        i++;
    }
    if (a[i] == b[i]) return 0;
    return (a[i] == '\0') ? -1 : 1;
}

size_t cj_str_repeat_b(char *dst, size_t cap, const char *src, size_t times) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!src || src[0] == '\0' || times == 0u) return 0u;
    size_t pos = 0u;
    for (size_t t = 0u; t < times; t++) {
        pos = cj_str_cat_b(dst, cap, src);
        if (pos >= cap - 1u) break;
    }
    return pos;
}

size_t cj_str_reverse_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    if (!src) {
        dst[0] = '\0';
        return 0u;
    }
    size_t len = strlen(src);
    size_t out_len = (len >= cap) ? (cap - 1u) : len;
    for (size_t i = 0u; i < out_len; i++) {
        dst[i] = src[len - 1u - i];
    }
    dst[out_len] = '\0';
    return out_len;
}

size_t cj_str_to_lower_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_copy_b(dst, cap, src);
    for (size_t i = 0u; i < n; i++) dst[i] = cj_char_to_lower(dst[i]);
    return n;
}

size_t cj_str_to_upper_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_copy_b(dst, cap, src);
    for (size_t i = 0u; i < n; i++) dst[i] = cj_char_to_upper(dst[i]);
    return n;
}

size_t cj_str_capitalize_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_to_lower_b(dst, cap, src);
    if (n > 0u) dst[0] = cj_char_to_upper(dst[0]);
    return n;
}

size_t cj_str_title_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_to_lower_b(dst, cap, src);
    int new_word = 1;
    for (size_t i = 0u; i < n; i++) {
        if (cj_char_is_space(dst[i])) {
            new_word = 1;
        } else if (new_word) {
            dst[i] = cj_char_to_upper(dst[i]);
            new_word = 0;
        }
    }
    return n;
}

size_t cj_str_swapcase_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_copy_b(dst, cap, src);
    for (size_t i = 0u; i < n; i++) {
        if (dst[i] >= 'a' && dst[i] <= 'z') dst[i] = cj_char_to_upper(dst[i]);
        else if (dst[i] >= 'A' && dst[i] <= 'Z') dst[i] = cj_char_to_lower(dst[i]);
    }
    return n;
}

int cj_str_is_lower(const char *s) {
    if (!s || s[0] == '\0') return 0;
    int has_alpha = 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') return 0;
        if (s[i] >= 'a' && s[i] <= 'z') has_alpha = 1;
    }
    return has_alpha;
}

int cj_str_is_upper(const char *s) {
    if (!s || s[0] == '\0') return 0;
    int has_alpha = 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') return 0;
        if (s[i] >= 'A' && s[i] <= 'Z') has_alpha = 1;
    }
    return has_alpha;
}

size_t cj_str_slugify_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!src) return 0u;
    size_t pos = 0u;
    int prev_dash = 0;
    for (size_t i = 0u; src[i] != '\0' && pos + 1u < cap; i++) {
        char c = src[i];
        if (cj_char_is_alnum(c)) {
            dst[pos++] = cj_char_to_lower(c);
            prev_dash = 0;
        } else if ((c == ' ' || c == '-' || c == '_') && pos > 0u && !prev_dash) {
            dst[pos++] = '-';
            prev_dash = 1;
        }
    }
    if (pos > 0u && dst[pos - 1u] == '-') pos--;
    dst[pos] = '\0';
    return pos;
}

int cj_char_is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') ? 1 : 0;
}

size_t cj_str_lstrip_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    if (!src) {
        dst[0] = '\0';
        return 0u;
    }
    size_t i = 0u;
    while (src[i] != '\0' && cj_char_is_space(src[i])) i++;
    return cj_str_copy_b(dst, cap, src + i);
}

size_t cj_str_rstrip_b(char *dst, size_t cap, const char *src) {
    size_t n = cj_str_copy_b(dst, cap, src);
    while (n > 0u && cj_char_is_space(dst[n - 1u])) {
        dst[--n] = '\0';
    }
    return n;
}

size_t cj_str_strip_b(char *dst, size_t cap, const char *src) {
    char tmp[1024];
    cj_str_lstrip_b(tmp, sizeof(tmp), src);
    return cj_str_rstrip_b(dst, cap, tmp);
}

size_t cj_str_collapse_ws_b(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!src) return 0u;
    size_t i = 0u;
    while (src[i] != '\0' && cj_char_is_space(src[i])) i++;
    size_t pos = 0u;
    int in_ws = 0;
    for (; src[i] != '\0' && pos + 1u < cap; i++) {
        if (cj_char_is_space(src[i])) {
            in_ws = 1;
        } else {
            if (in_ws && pos > 0u) {
                dst[pos++] = ' ';
                if (pos + 1u >= cap) break;
            }
            dst[pos++] = src[i];
            in_ws = 0;
        }
    }
    dst[pos] = '\0';
    return pos;
}

size_t cj_str_strip_char_b(char *dst, size_t cap, const char *src, char ch) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!src) return 0u;
    size_t pos = 0u;
    for (size_t i = 0u; src[i] != '\0' && pos + 1u < cap; i++) {
        if (src[i] != ch) dst[pos++] = src[i];
    }
    dst[pos] = '\0';
    return pos;
}

size_t cj_str_pad_left_b(char *dst, size_t cap, const char *src, size_t width, char pad) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    size_t slen = cj_str_len_s(src);
    size_t target = (width < cap) ? width : (cap - 1u);
    size_t pos = 0u;
    while (pos + slen < target && pos + 1u < cap) {
        dst[pos++] = pad;
    }
    dst[pos] = '\0';
    return cj_str_cat_b(dst, cap, src ? src : "");
}

size_t cj_str_pad_right_b(char *dst, size_t cap, const char *src, size_t width, char pad) {
    if (!dst || cap == 0u) return 0u;
    size_t pos = cj_str_copy_b(dst, cap, src);
    size_t target = (width < cap) ? width : (cap - 1u);
    while (pos < target && pos + 1u < cap) {
        dst[pos++] = pad;
    }
    dst[pos] = '\0';
    return pos;
}

size_t cj_str_center_b(char *dst, size_t cap, const char *src, size_t width, char pad) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    size_t slen = cj_str_len_s(src);
    size_t target = (width < cap) ? width : (cap - 1u);
    if (slen >= target) return cj_str_copy_b(dst, cap, src);
    size_t left_pad = (target - slen) / 2u;
    size_t pos = 0u;
    while (pos < left_pad && pos + 1u < cap) {
        dst[pos++] = pad;
    }
    dst[pos] = '\0';
    pos = cj_str_cat_b(dst, cap, src ? src : "");
    while (pos < target && pos + 1u < cap) {
        dst[pos++] = pad;
    }
    dst[pos] = '\0';
    return pos;
}

size_t cj_str_count_ws(const char *s) {
    if (!s) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (cj_char_is_space(s[i])) cnt++;
    }
    return cnt;
}

int cj_str_starts_with(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    size_t plen = strlen(prefix);
    if (strlen(s) < plen) return 0;
    return strncmp(s, prefix, plen) == 0 ? 1 : 0;
}

int cj_str_ends_with(const char *s, const char *suffix) {
    if (!s || !suffix) return 0;
    size_t slen = strlen(s);
    size_t xlen = strlen(suffix);
    if (slen < xlen) return 0;
    return strcmp(s + (slen - xlen), suffix) == 0 ? 1 : 0;
}

int cj_str_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    return strstr(haystack, needle) != NULL ? 1 : 0;
}

int cj_str_find_char(const char *s, char ch) {
    if (!s) return -1;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] == ch) return (int)i;
    }
    return -1;
}

int cj_str_rfind_char(const char *s, char ch) {
    if (!s) return -1;
    int last = -1;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] == ch) last = (int)i;
    }
    return last;
}

int cj_str_find_sub(const char *haystack, const char *needle) {
    if (!haystack || !needle || needle[0] == '\0') return -1;
    const char *p = strstr(haystack, needle);
    if (!p) return -1;
    return (int)(p - haystack);
}

size_t cj_str_count_char(const char *s, char ch) {
    if (!s) return 0u;
    size_t cnt = 0u;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] == ch) cnt++;
    }
    return cnt;
}

size_t cj_str_count_sub(const char *haystack, const char *needle) {
    if (!haystack || !needle || needle[0] == '\0') return 0u;
    size_t nlen = strlen(needle);
    size_t cnt = 0u;
    const char *p = haystack;
    while ((p = strstr(p, needle)) != NULL) {
        cnt++;
        p += nlen;
    }
    return cnt;
}

size_t cj_str_slice_b(char *dst, size_t cap, const char *src, size_t start, size_t end) {
    if (!dst || cap == 0u) return 0u;
    dst[0] = '\0';
    if (!src || start >= end) return 0u;
    size_t slen = strlen(src);
    if (start >= slen) return 0u;
    if (end > slen) end = slen;
    size_t len = end - start;
    if (len >= cap) len = cap - 1u;
    memcpy(dst, src + start, len);
    dst[len] = '\0';
    return len;
}

size_t cj_str_replace_first_b(char *dst, size_t cap, const char *src, const char *old_s, const char *new_s) {
    if (!dst || cap == 0u) return 0u;
    if (!src || !old_s || old_s[0] == '\0' || !new_s) {
        return cj_str_copy_b(dst, cap, src);
    }
    const char *pos = strstr(src, old_s);
    if (!pos) return cj_str_copy_b(dst, cap, src);

    size_t prefix_len = (size_t)(pos - src);
    size_t old_len = strlen(old_s);
    cj_str_slice_b(dst, cap, src, 0u, prefix_len);
    cj_str_cat_b(dst, cap, new_s);
    return cj_str_cat_b(dst, cap, pos + old_len);
}

int cj_char_is_digit(char c) { return (c >= '0' && c <= '9') ? 1 : 0; }
int cj_char_is_alpha(char c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) ? 1 : 0; }
int cj_char_is_alnum(char c) { return (cj_char_is_alpha(c) || cj_char_is_digit(c)) ? 1 : 0; }

int cj_str_is_digits(const char *s) {
    if (!s || s[0] == '\0') return 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (!cj_char_is_digit(s[i])) return 0;
    }
    return 1;
}

int cj_str_is_alpha(const char *s) {
    if (!s || s[0] == '\0') return 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (!cj_char_is_alpha(s[i])) return 0;
    }
    return 1;
}

int cj_str_is_alnum(const char *s) {
    if (!s || s[0] == '\0') return 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (!cj_char_is_alnum(s[i])) return 0;
    }
    return 1;
}

size_t cj_str_count_lines(const char *s) {
    if (!s || s[0] == '\0') return 0u;
    size_t lines = 1u;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (s[i] == '\n') lines++;
    }
    return lines;
}

size_t cj_str_count_words(const char *s) {
    if (!s) return 0u;
    size_t words = 0u;
    int in_word = 0;
    for (size_t i = 0u; s[i] != '\0'; i++) {
        if (cj_char_is_space(s[i])) {
            in_word = 0;
        } else if (!in_word) {
            words++;
            in_word = 1;
        }
    }
    return words;
}

size_t cj_str_from_int_b(char *dst, size_t cap, long long val) {
    if (!dst || cap == 0u) return 0u;
    int n = snprintf(dst, cap, "%lld", val);
    if (n < 0) {
        dst[0] = '\0';
        return 0u;
    }
    return ((size_t)n >= cap) ? (cap - 1u) : (size_t)n;
}

int cj_str_to_int_safe(const char *s, long long *out_val, long long fallback) {
    if (!s || s[0] == '\0') {
        if (out_val) *out_val = fallback;
        return -1;
    }
    size_t i = 0u;
    if (s[i] == '+' || s[i] == '-') i++;
    if (s[i] == '\0') {
        if (out_val) *out_val = fallback;
        return -1;
    }
    for (; s[i] != '\0'; i++) {
        if (!cj_char_is_digit(s[i])) {
            if (out_val) *out_val = fallback;
            return -1;
        }
    }
    long long v = strtoll(s, NULL, 10);
    if (out_val) *out_val = v;
    return 0;
}
