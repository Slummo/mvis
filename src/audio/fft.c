#include "fft.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

static inline size_t log2ld(size_t n) {
    size_t i = 0;
    for (i = 0; n; ++i) {
        n >>= 1;
    }
    return i - 1;
}

static inline size_t bit_reverse(size_t n, size_t m) {
    size_t r = 0;
    size_t l = log2ld(m);
    for (size_t i = 0; i < l; ++i) {
        if ((n >> i) & 1) {
            r |= (1ul << (l - 1 - i));
        }
    }
    return r;
}

int fft(size_t n, double complex samples[static n]) {
    if (!check_pow2(n)) {
        return 1;
    }

// Reorder the array using bitwise-reversed indices
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) {
        size_t j = bit_reverse(i, n);
        if (i < j) {
            double complex temp = samples[i];
            samples[i] = samples[j];
            samples[j] = temp;
        }
    }

    const size_t l = log2ld(n);
    const double complex f = -2 * M_PI * I;
    for (size_t i = 1; i <= l; ++i) {
        size_t m = 1ull << i;  // 2^i
        double complex wm = cexp(f / m);

#pragma omp parallel for
        for (size_t j = 0; j < n; j += m) {
            double complex w = 1;
            for (size_t k = 0; k <= m / 2 - 1; ++k) {
                double complex t = w * samples[j + k + m / 2];
                double complex u = samples[j + k];
                samples[j + k] = u + t;
                samples[j + k + m / 2] = u - t;
                w *= wm;
            }
        }
    }

    return 0;
}