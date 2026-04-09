#pragma once

#include <stddef.h>
#include <stdint.h>
#include <complex.h>

typedef struct stream stream;

stream* stream_new(const char* filename, size_t nof_samples);
uint32_t stream_sleep_time(const stream* stream);
int stream_read(stream* stream);
void stream_transform(stream* stream);
void stream_rewind(stream* stream, int64_t samples_off);
double* stream_frequencies(const stream* stream);
void stream_free(stream** stream_ptr);