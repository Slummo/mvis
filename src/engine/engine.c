#include <mvis/engine.h>
#include <mvis/log.h>
#include "stream.h"
#include "da.h"
#include "player.h"
#include "window/window.h"
#include "visualizer.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH (640)
#define HEIGHT (400)
#define TITLE ("MVIS")
#define NOF_SHAPES (1ull << 7)

da_define(stream);

struct engine {
    stream_da streams;
    player player;
    GLFWwindow* window;
    visualizer visualizer;
};

engine* engine_new(void) {
    engine* engine = calloc(1, sizeof(struct engine));
    if (!engine) {
        ERR("OOM");
        return NULL;
    }

    da_init(engine->streams, 1);
    if (da_isnull(engine->streams)) {
        goto error;
    }

    if (player_init(&engine->player) != 0) {
        goto error;
    }

    engine->window = window_new(WIDTH, HEIGHT, TITLE);
    if (!engine->window) {
        goto error;
    }

    if (visualizer_init(&engine->visualizer, NOF_SHAPES) != 0) {
        goto error;
    }

    return engine;

error:
    window_free(engine->window);
    player_cleanup(&engine->player);
    da_free(engine->streams);
    free(engine);
    return NULL;
}

track_id engine_add(engine* engine, const char* filename, size_t win_len) {
    if (!engine || !filename) {
        return -1;
    }

    stream stream = {0};
    stream_init(&stream, filename, win_len);

    track_id id = da_len(engine->streams);
    da_append(engine->streams, stream);

    return id;
}

int engine_start(engine* engine, track_id id) {
    if (!engine || id < 0 || (size_t)id >= engine->streams.len) {
        return 1;
    }

    stream* track = &da_get(engine->streams, id);

    if (player_start(&engine->player, track) != 0) {
        ERR("player_start failed");
        return 1;
    }

    visualizer_set_shape(&engine->visualizer, SHAPE_RECTANGLE);
    visualizer_set_background(&engine->visualizer, (col4f){0.05f, 0.05f, 0.05f, 1.0f});
    visualizer_set_gradient(&engine->visualizer, NULL);  // TODO

    while (!window_should_close(engine->window) && !track->is_finished) {
        stream_transform(track);
        visualizer_set_heights(&engine->visualizer, (size_t)track->win_len, track->mag_buf);
        visualizer_draw(&engine->visualizer);

        swap_buffers(engine->window);
        poll_events();
    }

    player_stop(&engine->player);

    return 0;
}

int engine_pause(engine* engine, track_id id, size_t micro);
int engine_stop(engine* engine, track_id id);

void engine_free(engine* engine) {
    if (!engine) {
        return;
    }

    visualizer_cleanup(&engine->visualizer);
    window_free(engine->window);
    player_cleanup(&engine->player);

    for (size_t i = 0; i < da_len(engine->streams); ++i) {
        stream_cleanup(&da_get(engine->streams, i));
    }

    da_free(engine->streams);
    free(engine);
}