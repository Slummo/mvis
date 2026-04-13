#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <complex.h>
#include <sndfile.h>

typedef struct stream {
    SNDFILE* file;
    SF_INFO info;
    bool is_finished;

    int32_t win_len;
    void* buf;
    float* pcm_buf;
    float* mag_buf;
    float complex* fft_buf;
} stream;

int stream_init(stream* stream, const char* filename, int32_t win_len);
int64_t stream_read(stream* stream);
void stream_transform(stream* stream);
void stream_cleanup(stream* stream);