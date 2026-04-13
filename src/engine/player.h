#pragma once

#include <mvis/engine.h>
#include "stream.h"
#include <stdint.h>
#include <stdbool.h>

typedef void PaStream;

typedef struct player {
    PaStream* pa_stream;
    bool is_playing;
} player;

int player_init(player* player);
int player_start(player* player, stream* stream);
void player_sleep(uint32_t millisec);
int player_stop(player* player);
void player_cleanup(player* player);