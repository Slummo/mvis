#include "stream.h"
#include "fft/fft.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int stream_init(stream* stream, const char* filename, int32_t win_len) {
    if (!stream || !filename || win_len <= 0) {
        return 1;
    }

    SF_INFO info = {0};
    SNDFILE* file = sf_open(filename, SFM_READ, &info);
    if (!file) {
        return 1;
    }

    size_t pcm_size = sizeof(float) * (size_t)win_len * (size_t)info.channels;
    size_t mag_size = sizeof(float) * (size_t)win_len;
    size_t fft_size = sizeof(float complex) * (size_t)win_len;

    uint8_t* buf = malloc(pcm_size + mag_size + fft_size);
    if (!buf) {
        sf_close(file);
        return 1;
    }

    stream->file = file;
    stream->info = info;
    stream->is_finished = false;
    stream->win_len = win_len;
    stream->buf = buf;
    stream->pcm_buf = (float*)buf;
    stream->mag_buf = (float*)(buf + pcm_size);
    stream->fft_buf = (float complex*)(buf + pcm_size + mag_size);

    return 0;
}

int64_t stream_read(stream* stream) {
    if (!stream) {
        return -1;
    }

    return sf_readf_float(stream->file, stream->pcm_buf, (sf_count_t)stream->win_len);
}

void stream_transform(stream* stream) {
    assert(stream);

    // Convert the averages of the two channels to complex samples,
    // applying hanning window
    for (int32_t i = 0; i < stream->win_len; ++i) {
        float mono = stream->info.channels == 2 ? (stream->pcm_buf[i * 2] + stream->pcm_buf[i * 2 + 1]) / 2.0
                                                : stream->pcm_buf[i];
        float window = 0.5 * (1.0 - cos(2.0 * M_PI * i / (stream->win_len - 1)));
        stream->fft_buf[i] = mono * window + 0.0 * I;
    }

    int res = fft((size_t)stream->win_len, stream->fft_buf);
    assert(res == 0);

    // Convert to logarithmic scale
    const float fs = stream->info.samplerate;
    const float norm_factor = 2.0f / stream->win_len;
    static const float db_floor = -60.0f;

    for (int32_t i = 0; i < stream->win_len; ++i) {
        float m = norm_factor * cabsf(stream->fft_buf[i]);
        if (m < 0.001f) {
            m = 0.001f;
        }

        float db = 20.0f * log10f(m);
        float f = (db - db_floor) / (-db_floor);
        if (f < 0.0f) {
            f = 0.0f;
        }

        stream->mag_buf[i] = f;
    }
}

void stream_cleanup(stream* stream) {
    if (!stream) {
        return;
    }

    free(stream->buf);
    sf_close(stream->file);
    memset(stream, 0, sizeof(struct stream));
}