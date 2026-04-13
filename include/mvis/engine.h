#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct engine engine;
typedef int32_t track_id;

engine* engine_new(void);
track_id engine_add(engine* engine, const char* filename, size_t win_len);
int engine_start(engine* engine, track_id id);
int engine_pause(engine* engine, track_id id, size_t micro);
int engine_stop(engine* engine, track_id id);
void engine_free(engine* engine);