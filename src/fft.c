#include <mvis/fft.h>

#include <stddef.h>
#include <stdlib.h>
#include <omp.h>

#define PI (3.14159265358979323846)

static inline uint64_t log2ld(uint64_t n) {
    uint64_t i = 0;
    for (i = 0; n; ++i) {
        n >>= 1;
    }
    return i - 1;
}

static inline int32_t check_pow2(uint64_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static inline uint64_t bit_reverse(const uint64_t num, const uint64_t N) {
    uint64_t r = 0;
    uint64_t l = log2ld(N);
    for (uint64_t i = 0; i < l; ++i) {
        if ((num >> i) & 1) {
            r |= (1ul << (l - 1 - i));
        }
    }
    return r;
}

double complex* fft(const double complex* samples, const uint64_t nof_samples) {
    double complex* freqs = malloc(sizeof(double complex) * nof_samples);
    if (!samples || !freqs) {
        return NULL;
    }

    if (!check_pow2(nof_samples)) {
        return NULL;
    }

// Reorder the array using bitwise-reversed indices
#pragma omp parallel for
    for (uint64_t i = 0; i < nof_samples; ++i) {
        freqs[bit_reverse(i, nof_samples)] = samples[i];
    }

    uint64_t l = log2ld(nof_samples);
    for (uint64_t i = 1; i <= l; ++i) {
        uint64_t m = 1ul << i;  // 2^i
        double complex wm = cexp(-2 * PI * I / m);

#pragma omp parallel for
        for (uint64_t j = 0; j < nof_samples; j += m) {
            double complex w = 1;
            for (uint64_t k = 0; k <= m / 2 - 1; ++k) {
                double complex t = w * freqs[j + k + m / 2];
                double complex u = freqs[j + k];
                freqs[j + k] = u + t;
                freqs[j + k + m / 2] = u - t;
                w *= wm;
            }
        }
    }

    return freqs;
}