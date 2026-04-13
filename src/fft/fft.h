#pragma once

#include <stddef.h>
#include <complex.h>

static inline int check_pow2(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int fft(size_t n, float complex samples[static n]);