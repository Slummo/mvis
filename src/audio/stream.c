#include <mvis/audio.h>
#include "fft.h"
#include <mvis/log.h>

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>

struct stream {
    SNDFILE* file;
    SF_INFO info;
    size_t nof_samples;
    double* samples;
    double complex* freqs;
};

stream* stream_new(const char* filename, size_t nof_samples) {
    if (!filename || !check_pow2(nof_samples)) {
        return NULL;
    }

    stream* stream = NULL;
    SNDFILE* file = NULL;
    double* samples = NULL;
    double complex* freqs = NULL;

    stream = malloc(sizeof(struct stream));
    if (!stream) {
        ERR("OOM");
        goto error;
    }

    // Open audio file
    SF_INFO i;
    file = sf_open(filename, SFM_READ, &i);
    if (sf_error(file) != SF_ERR_NO_ERROR) {
        ERR("Error with sf_open: %s", sf_strerror(file));
        goto error;
    }

    samples = malloc(sizeof(double) * nof_samples * (size_t)i.channels);
    freqs = malloc(sizeof(double complex) * nof_samples);  // single channel
    if (!samples || !freqs) {
        ERR("OOM");
        goto error;
    }

    goto success;

error:
    free(freqs);
    free(samples);
    sf_close(file);
    free(stream);
    return NULL;

success:
    stream->file = file;
    stream->info = i;
    stream->nof_samples = nof_samples;
    stream->samples = samples;
    stream->freqs = freqs;
    return stream;
}

uint32_t stream_sleep_time(const stream* stream) {
    const double fps = stream->nof_samples / 2.0;
    const double s = fps / stream->info.samplerate;
    return (uint32_t)(s * 1e6);
}

int stream_read(stream* stream) {
    if (!stream) {
        return 1;
    }

    // Sample n real values from the stream
    size_t read = (size_t)sf_readf_double(stream->file, stream->samples, (sf_count_t)stream->nof_samples);
    if (read == 0) {
        return 1;
    }

    // Convert the averages of the two channels to complex samples,
    // applying hanning window
    for (size_t i = 0; i < stream->nof_samples; ++i) {
        double mono = stream->info.channels == 2 ? (stream->samples[i * 2] + stream->samples[i * 2 + 1]) / 2.0
                                                 : stream->samples[i];
        double window = 0.5 * (1.0 - cos(2.0 * M_PI * i / (stream->nof_samples - 1)));
        stream->freqs[i] = mono * window + 0.0 * I;
    }

    return 0;
}

void stream_transform(stream* stream) {
    assert(stream);
    int res = fft(stream->nof_samples, stream->freqs);
    assert(res == 0);
}

void stream_rewind(stream* stream, int64_t samples_off) {
    assert(stream);
    sf_seek(stream->file, samples_off, SEEK_CUR);
}

double* stream_frequencies(const stream* stream) {
    assert(stream);

    const double fs = stream->info.samplerate;
    const double norm_factor = 2.0 / stream->nof_samples;
    static const double db_floor = -60.0;

    for (size_t i = 0; i < stream->nof_samples; ++i) {
        double m = norm_factor * cabs(stream->freqs[i]);
        if (m < 0.001) {
            m = 0.001;
        }

        double db = 20.0 * log10(m);
        double f = (db - db_floor) / (-db_floor);
        if (f < 0.0) {
            f = 0.0;
        }

        stream->samples[i] = f;
    }

    return stream->samples;
}

void stream_free(stream** stream_ptr) {
    if (!stream_ptr) {
        return;
    }

    stream* s = *stream_ptr;
    if (!s) {
        return;
    }

    free(s->freqs);
    free(s->samples);
    sf_close(s->file);
    free(s);

    *stream_ptr = NULL;
}