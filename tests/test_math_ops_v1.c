#include "cj_math_ops_v1.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static int approx_eq(double a, double b) {
    return fabs(a - b) < 1e-6;
}

int main(void) {
    assert(cj_add_ll(10, 20) == 30);
    assert(cj_sub_ll(20, 7) == 13);
    assert(cj_mul_ll(6, 7) == 42);
    assert(cj_div_ll_safe(20, 0, -1) == -1);
    assert(cj_mod_ll_safe(10, 3, -1) == 1);
    assert(cj_abs_ll(-45) == 45);
    assert(cj_sign_ll(-9) == -1 && cj_sign_ll(0) == 0 && cj_sign_ll(5) == 1);
    assert(cj_min_ll(3, 8) == 3 && cj_max_ll(3, 8) == 8);
    assert(cj_clamp_ll(50, 0, 10) == 10);

    assert(cj_pow_ll(2, 10) == 1024);
    assert(cj_pow_mod_ll(3, 4, 5) == 1);
    assert(cj_isqrt_ll(81) == 9);
    assert(cj_gcd_ll(48, 18) == 6);
    assert(cj_lcm_ll(4, 6) == 12);
    assert(cj_is_even_ll(8) == 1 && cj_is_odd_ll(7) == 1);
    assert(cj_is_prime_ll(29) == 1 && cj_is_prime_ll(21) == 0);
    assert(cj_factorial_u(5u) == 120);
    assert(cj_fibonacci_u(7u) == 13);

    assert(cj_digit_count_ll(12345) == 5);
    assert(cj_digit_sum_ll(1234) == 10);
    assert(cj_reverse_digits_ll(123) == 321);
    assert(cj_is_palindrome_ll(1221) == 1);
    assert(cj_sum_to_n_ll(10) == 55);
    assert(cj_sum_squares_to_n_ll(3) == 14);
    assert(cj_comb_u(5u, 2u) == 10);
    assert(cj_perm_u(5u, 2u) == 20);
    assert(cj_triangular_u(4u) == 10);
    assert(cj_tetrahedral_u(3u) == 10);

    double arr[4] = {2.0, 4.0, 6.0, 8.0};
    assert(approx_eq(cj_avg2_d(2.0, 6.0), 4.0));
    assert(approx_eq(cj_avg3_d(3.0, 6.0, 9.0), 6.0));
    assert(approx_eq(cj_sum_array_d(arr, 4), 20.0));
    assert(approx_eq(cj_avg_array_d(arr, 4), 5.0));
    assert(approx_eq(cj_min_array_d(arr, 4), 2.0));
    assert(approx_eq(cj_max_array_d(arr, 4), 8.0));
    assert(approx_eq(cj_lerp_d(0.0, 100.0, 0.25), 25.0));
    assert(approx_eq(cj_inv_lerp_d(0.0, 100.0, 25.0), 0.25));
    assert(approx_eq(cj_percent_of_d(25.0, 200.0), 12.5));
    assert(approx_eq(cj_apply_percent_d(100.0, 15.0), 115.0));

    unsigned long long b = cj_bit_set_ull(0ull, 3u);
    assert(cj_bit_test_ull(b, 3u) == 1);
    b = cj_bit_toggle_ull(b, 3u);
    assert(cj_bit_test_ull(b, 3u) == 0);
    b = cj_bit_set_ull(b, 1u);
    b = cj_bit_clear_ull(b, 1u);
    assert(cj_popcount_ull(7ull) == 3u);
    assert(cj_is_pow2_ull(16ull) == 1 && cj_is_pow2_ull(12ull) == 0);
    assert(cj_next_pow2_ull(5ull) == 8ull);
    assert(cj_round_to_mult_ll(14, 5) == 15);
    assert(cj_floor_div_ll(-7, 2) == -4);
    assert(cj_ceil_div_ll(7, 2) == 4);

    printf("OK: cj_math_ops_v1 (50 functions) passed\n");
    return 0;
}
