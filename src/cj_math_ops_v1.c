#include "cj_math_ops_v1.h"

long long cj_add_ll(long long a, long long b) { return a + b; }
long long cj_sub_ll(long long a, long long b) { return a - b; }
long long cj_mul_ll(long long a, long long b) { return a * b; }

long long cj_div_ll_safe(long long a, long long b, long long fallback) {
    if (b == 0) return fallback;
    return a / b;
}

long long cj_mod_ll_safe(long long a, long long b, long long fallback) {
    if (b == 0) return fallback;
    return a % b;
}

long long cj_abs_ll(long long x) {
    return (x < 0) ? -x : x;
}

long long cj_sign_ll(long long x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

long long cj_min_ll(long long a, long long b) { return (a < b) ? a : b; }
long long cj_max_ll(long long a, long long b) { return (a > b) ? a : b; }

long long cj_clamp_ll(long long x, long long lo, long long hi) {
    if (lo > hi) {
        long long tmp = lo;
        lo = hi;
        hi = tmp;
    }
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

long long cj_pow_ll(long long base, unsigned int exp) {
    long long res = 1;
    long long cur = base;
    while (exp > 0u) {
        if (exp & 1u) res *= cur;
        exp >>= 1u;
        if (exp > 0u) cur *= cur;
    }
    return res;
}

long long cj_pow_mod_ll(long long base, unsigned long long exp, long long mod) {
    if (mod <= 0) return 0;
    long long res = 1 % mod;
    long long cur = base % mod;
    if (cur < 0) cur += mod;
    while (exp > 0ull) {
        if (exp & 1ull) res = (res * cur) % mod;
        exp >>= 1ull;
        if (exp > 0ull) cur = (cur * cur) % mod;
    }
    return res;
}

long long cj_isqrt_ll(long long n) {
    if (n <= 0) return 0;
    long long lo = 1, hi = (n < 3037000499LL) ? n : 3037000499LL, ans = 0;
    while (lo <= hi) {
        long long mid = lo + (hi - lo) / 2;
        long long sq = mid * mid;
        if (sq == n) return mid;
        if (sq < n) {
            ans = mid;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return ans;
}

long long cj_gcd_ll(long long a, long long b) {
    long long x = cj_abs_ll(a);
    long long y = cj_abs_ll(b);
    while (y != 0) {
        long long t = x % y;
        x = y;
        y = t;
    }
    return x;
}

long long cj_lcm_ll(long long a, long long b) {
    long long g = cj_gcd_ll(a, b);
    if (g == 0) return 0;
    return (cj_abs_ll(a) / g) * cj_abs_ll(b);
}

int cj_is_even_ll(long long n) { return ((n & 1LL) == 0) ? 1 : 0; }
int cj_is_odd_ll(long long n)  { return ((n & 1LL) != 0) ? 1 : 0; }

int cj_is_prime_ll(long long n) {
    if (n < 2) return 0;
    if (n == 2 || n == 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

long long cj_factorial_u(unsigned int n) {
    if (n > 20u) return 0;
    long long res = 1;
    for (unsigned int i = 2u; i <= n; i++) {
        res *= (long long)i;
    }
    return res;
}

long long cj_fibonacci_u(unsigned int n) {
    if (n == 0u) return 0;
    if (n == 1u) return 1;
    long long a = 0, b = 1;
    for (unsigned int i = 2u; i <= n; i++) {
        long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int cj_digit_count_ll(long long n) {
    if (n == 0) return 1;
    long long v = cj_abs_ll(n);
    int cnt = 0;
    while (v > 0) {
        cnt++;
        v /= 10;
    }
    return cnt;
}

int cj_digit_sum_ll(long long n) {
    long long v = cj_abs_ll(n);
    int sum = 0;
    while (v > 0) {
        sum += (int)(v % 10);
        v /= 10;
    }
    return sum;
}

long long cj_reverse_digits_ll(long long n) {
    int neg = (n < 0) ? 1 : 0;
    long long v = cj_abs_ll(n);
    long long rev = 0;
    while (v > 0) {
        rev = rev * 10 + (v % 10);
        v /= 10;
    }
    return neg ? -rev : rev;
}

int cj_is_palindrome_ll(long long n) {
    if (n < 0) return 0;
    return (cj_reverse_digits_ll(n) == n) ? 1 : 0;
}

long long cj_sum_to_n_ll(long long n) {
    if (n <= 0) return 0;
    return (n * (n + 1)) / 2;
}

long long cj_sum_squares_to_n_ll(long long n) {
    if (n <= 0) return 0;
    return (n * (n + 1) * (2 * n + 1)) / 6;
}

long long cj_comb_u(unsigned int n, unsigned int r) {
    if (r > n) return 0;
    if (r == 0u || r == n) return 1;
    if (r > n - r) r = n - r;
    long long num = 1, den = 1;
    for (unsigned int i = 1u; i <= r; i++) {
        num *= (long long)(n - r + i);
        den *= (long long)i;
        long long g = cj_gcd_ll(num, den);
        num /= g;
        den /= g;
    }
    return num / den;
}

long long cj_perm_u(unsigned int n, unsigned int r) {
    if (r > n) return 0;
    long long res = 1;
    for (unsigned int i = 0u; i < r; i++) {
        res *= (long long)(n - i);
    }
    return res;
}

long long cj_triangular_u(unsigned int n) {
    return cj_sum_to_n_ll((long long)n);
}

long long cj_tetrahedral_u(unsigned int n) {
    long long x = (long long)n;
    return (x * (x + 1) * (x + 2)) / 6;
}

double cj_avg2_d(double a, double b) { return (a + b) * 0.5; }
double cj_avg3_d(double a, double b, double c) { return (a + b + c) / 3.0; }

double cj_sum_array_d(const double *arr, size_t n) {
    if (!arr || n == 0) return 0.0;
    double s = 0.0;
    for (size_t i = 0; i < n; i++) s += arr[i];
    return s;
}

double cj_avg_array_d(const double *arr, size_t n) {
    if (!arr || n == 0) return 0.0;
    return cj_sum_array_d(arr, n) / (double)n;
}

double cj_min_array_d(const double *arr, size_t n) {
    if (!arr || n == 0) return 0.0;
    double m = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < m) m = arr[i];
    }
    return m;
}

double cj_max_array_d(const double *arr, size_t n) {
    if (!arr || n == 0) return 0.0;
    double m = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > m) m = arr[i];
    }
    return m;
}

double cj_lerp_d(double a, double b, double t) {
    return a + (b - a) * t;
}

double cj_inv_lerp_d(double a, double b, double v) {
    if (b == a) return 0.0;
    return (v - a) / (b - a);
}

double cj_percent_of_d(double part, double total) {
    if (total == 0.0) return 0.0;
    return (part / total) * 100.0;
}

double cj_apply_percent_d(double base, double pct) {
    return base * (1.0 + pct / 100.0);
}

unsigned long long cj_bit_set_ull(unsigned long long x, unsigned int bit) {
    if (bit >= 64u) return x;
    return x | (1ull << bit);
}

unsigned long long cj_bit_clear_ull(unsigned long long x, unsigned int bit) {
    if (bit >= 64u) return x;
    return x & ~(1ull << bit);
}

unsigned long long cj_bit_toggle_ull(unsigned long long x, unsigned int bit) {
    if (bit >= 64u) return x;
    return x ^ (1ull << bit);
}

int cj_bit_test_ull(unsigned long long x, unsigned int bit) {
    if (bit >= 64u) return 0;
    return ((x >> bit) & 1ull) ? 1 : 0;
}

unsigned int cj_popcount_ull(unsigned long long x) {
    unsigned int cnt = 0;
    while (x > 0ull) {
        x &= (x - 1ull);
        cnt++;
    }
    return cnt;
}

int cj_is_pow2_ull(unsigned long long x) {
    return (x > 0ull && (x & (x - 1ull)) == 0ull) ? 1 : 0;
}

unsigned long long cj_next_pow2_ull(unsigned long long x) {
    if (x <= 1ull) return 1ull;
    unsigned long long p = 1ull;
    while (p < x && p <= (1ull << 62)) {
        p <<= 1ull;
    }
    return p;
}

long long cj_round_to_mult_ll(long long x, long long step) {
    if (step == 0) return x;
    long long s = cj_abs_ll(step);
    if (x >= 0) return ((x + s / 2) / s) * s;
    return -(((-x + s / 2) / s) * s);
}

long long cj_floor_div_ll(long long a, long long b) {
    if (b == 0) return 0;
    long long q = a / b;
    long long r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) q--;
    return q;
}

long long cj_ceil_div_ll(long long a, long long b) {
    if (b == 0) return 0;
    long long q = a / b;
    long long r = a % b;
    if (r != 0 && ((r > 0) == (b > 0))) q++;
    return q;
}
