#include <fft/fft.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NOF_SAMPLES (1ul << 12)
#define FREQ(hz) (2 * M_PI * hz)

static inline double signal1(double t) {
    return sin(FREQ(3.0) * t);
}

static inline double signal2(double t) {
    return sin(FREQ(10.0) * t) + cos(FREQ(2.0) * t) + sin(FREQ(5.0) * t);
}

typedef double (*signal)(double t);

static double complex* sample(signal g, const uint64_t N, const double fs) {
    double complex* samples = malloc(sizeof(double complex) * N);
    if (!samples) {
        return NULL;
    }

    const double dt = 1.0 / fs;
    for (uint64_t i = 0; i < N; ++i) {
        double t = i * dt;
        samples[i] = g(t) + 0.0 * I;
    }

    return samples;
}

int main(void) {
    const double fs = 30.0;  // sampling frequency
    double complex* samples = sample(signal2, NOF_SAMPLES, fs);
    double complex* freqs = fft(samples, NOF_SAMPLES);

    for (uint64_t i = 0; i < NOF_SAMPLES; ++i) {
        double f = fs * (double)i / (double)NOF_SAMPLES;
        // double m = 2.0 * cabs(freqs[i]) / (double)NOF_SAMPLES;  // normalize magnitudes
        double m = cabs(freqs[i]);
        printf("%lf %lf\n", f, m);
    }

    free(samples);
    free(freqs);
    return 0;
}