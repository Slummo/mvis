#include "player.h"
#include <mvis/log.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <portaudio.h>

static int pa_callback(const void* input, void* output, unsigned long frame_count,
                       const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flag, void* user_data) {
    stream* stream = user_data;
    float* out = output;
    const float* pcm = stream->pcm_buf;

    size_t sample_count = frame_count * (size_t)stream->info.channels;

    if (stream_read(stream) <= 0) {
        stream->is_finished = true;
        memset(out, 0, sizeof(float) * sample_count);
        return paComplete;
    }

    for (size_t i = 0; i < sample_count; ++i) {
        out[i] = pcm[i];
    }

    return paContinue;
}

int player_init(player* player) {
    if (!player) {
        return 1;
    }

    PaError res = Pa_Initialize();
    if (res != paNoError) {
        ERR("Error with Pa_Initialize: %s", Pa_GetErrorText(res));
        return 1;
    }

    return 0;
}

int player_start(player* player, stream* stream) {
    if (!player) {
        return 1;
    }

    PaError res = Pa_OpenDefaultStream(&player->pa_stream, 0, stream->info.channels, paFloat32, stream->info.samplerate,
                                       (unsigned long)stream->win_len, pa_callback, stream);
    if (res != paNoError) {
        ERR("Error with Pa_OpenDefaultStream: %s", Pa_GetErrorText(res));
        return 1;
    }

    res = Pa_StartStream(player->pa_stream);
    if (res != paNoError) {
        ERR("Error with Pa_StartStream: %s", Pa_GetErrorText(res));
        return 1;
    }

    player->is_playing = true;

    return 0;
}

void player_sleep(uint32_t msec) {
    Pa_Sleep((long)msec);
}

int player_stop(player* player) {
    PaError res = Pa_StopStream(player->pa_stream);
    if (res != paNoError) {
        ERR("Error with Pa_StopStream: %s", Pa_GetErrorText(res));
        return 1;
    }

    player->is_playing = false;

    return 0;
}

void player_cleanup(player* player) {
    if (!player) {
        return;
    }

    Pa_Terminate();
    memset(player, 0, sizeof(struct player));
}