#pragma once

#include <stdint.h>
#include <complex.h>

double complex* fft(const double complex* samples, const uint64_t nof_samples);