#include <mvis/track.h>
#include <mvis/log.h>
#include <mvis/fft.h>

#include <stdlib.h>

track* track_new(const char* filename) {
    track* t = calloc(1, sizeof(track));
    if (!t) {
        ERR("OOM");
        return NULL;
    }

    // Open audio file
    SF_INFO info;
    SNDFILE* file = sf_open(filename, SFM_READ, &info);
    if (sf_error(file) != SF_ERR_NO_ERROR) {
        ERR("Error with sf_open: %s", sf_strerror(file));
        free(t);
        return NULL;
    }
    t->file = file;
    t->info = info;

    // Allocate space for samples
    t->rsamples = malloc(sizeof(double) * NOF_SAMPLES * (size_t)info.channels);
    t->csamples = malloc(sizeof(double complex) * NOF_SAMPLES);  // single channel
    if (!t->rsamples || !t->csamples) {
        ERR("OOM");
        track_free(&t);
        return NULL;
    }

    return t;
}

double complex* track_read(track* track) {
    // Sample NOF_SAMPLES real values from the track
    if (sf_readf_double(track->file, track->rsamples, NOF_SAMPLES) != NOF_SAMPLES) {
        return NULL;
    }

    // Convert the averages of the two channels to complex values
    for (uint64_t i = 0; i < NOF_SAMPLES; ++i) {
        double mono = track->info.channels == 2 ? (track->rsamples[i * 2] + track->rsamples[i * 2 + 1]) / 2.0
                                                : track->rsamples[i];
        track->csamples[i] = mono + 0.0 * I;
    }

    double complex* freqs = fft(track->csamples, NOF_SAMPLES);
    if (!freqs) {
        ERR("Error with fft");
        return NULL;
    }

    return freqs;
}

void track_free(track** track_ptr) {
    if (!track_ptr || !*track_ptr) {
        return;
    }

    track* t = *track_ptr;

    sf_close(t->file);
    free(t->rsamples);
    free(t->csamples);
    free(t);

    *track_ptr = NULL;
}