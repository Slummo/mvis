#pragma once

#include <sndfile.h>
#include <complex.h>

#define NOF_SAMPLES (1ul << 7)

typedef struct track {
    SNDFILE* file;
    SF_INFO info;
    double* rsamples;
    double complex* csamples;
} track;

track* track_new(const char* filename);
double complex* track_read(track* track);
void track_free(track** track_ptr);