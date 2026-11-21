#ifndef CJ_MATH_OPS_V1_H
#define CJ_MATH_OPS_V1_H

#include <stddef.h>

/* 1-10: Basic arithmetic & bounds */
long long cj_add_ll(long long a, long long b);
long long cj_sub_ll(long long a, long long b);
long long cj_mul_ll(long long a, long long b);
long long cj_div_ll_safe(long long a, long long b, long long fallback);
long long cj_mod_ll_safe(long long a, long long b, long long fallback);
long long cj_abs_ll(long long x);
long long cj_sign_ll(long long x);
long long cj_min_ll(long long a, long long b);
long long cj_max_ll(long long a, long long b);
long long cj_clamp_ll(long long x, long long lo, long long hi);

/* 11-20: Powers, roots & number theory */
long long cj_pow_ll(long long base, unsigned int exp);
long long cj_pow_mod_ll(long long base, unsigned long long exp, long long mod);
long long cj_isqrt_ll(long long n);
long long cj_gcd_ll(long long a, long long b);
long long cj_lcm_ll(long long a, long long b);
int cj_is_even_ll(long long n);
int cj_is_odd_ll(long long n);
int cj_is_prime_ll(long long n);
long long cj_factorial_u(unsigned int n);
long long cj_fibonacci_u(unsigned int n);

/* 21-30: Digits & combinatorics */
int cj_digit_count_ll(long long n);
int cj_digit_sum_ll(long long n);
long long cj_reverse_digits_ll(long long n);
int cj_is_palindrome_ll(long long n);
long long cj_sum_to_n_ll(long long n);
long long cj_sum_squares_to_n_ll(long long n);
long long cj_comb_u(unsigned int n, unsigned int r);
long long cj_perm_u(unsigned int n, unsigned int r);
long long cj_triangular_u(unsigned int n);
long long cj_tetrahedral_u(unsigned int n);

/* 31-40: Stats, ratios, percentages & interpolation */
double cj_avg2_d(double a, double b);
double cj_avg3_d(double a, double b, double c);
double cj_avg_array_d(const double *arr, size_t n);
double cj_sum_array_d(const double *arr, size_t n);
double cj_min_array_d(const double *arr, size_t n);
double cj_max_array_d(const double *arr, size_t n);
double cj_lerp_d(double a, double b, double t);
double cj_inv_lerp_d(double a, double b, double v);
double cj_percent_of_d(double part, double total);
double cj_apply_percent_d(double base, double pct);

/* 41-50: Bitwise & rounding helpers */
unsigned long long cj_bit_set_ull(unsigned long long x, unsigned int bit);
unsigned long long cj_bit_clear_ull(unsigned long long x, unsigned int bit);
unsigned long long cj_bit_toggle_ull(unsigned long long x, unsigned int bit);
int cj_bit_test_ull(unsigned long long x, unsigned int bit);
unsigned int cj_popcount_ull(unsigned long long x);
int cj_is_pow2_ull(unsigned long long x);
unsigned long long cj_next_pow2_ull(unsigned long long x);
long long cj_round_to_mult_ll(long long x, long long step);
long long cj_floor_div_ll(long long a, long long b);
long long cj_ceil_div_ll(long long a, long long b);

#endif
