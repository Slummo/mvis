#include <mvis/window.h>
#include <mvis/audio.h>
#include <mvis/render.h>
#include <mvis/log.h>

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH (640)
#define HEIGHT (400)
#define TITLE ("MVIS")
#define NOF_SAMPLES (1ull << 10)
#define NOF_SHAPES (1ull << 6)

int main(int argc, char* argv[]) {
    int ret = EXIT_FAILURE;
    errno = 0;

    GLFWwindow* w = NULL;
    stream* s = NULL;
    visualizer* v = NULL;

    w = window_new(WIDTH, HEIGHT, TITLE);
    s = stream_new(argv[1], NOF_SAMPLES);
    v = visualizer_new(NOF_SHAPES);

    if (!w || !s || !v) {
        goto cleanup;
    }

    visualizer_set_background(v, (col4f){0.0f, 0.0f, 0.0f, 1.0f});
    visualizer_set_shape(v, SHAPE_RECTANGLE);
    visualizer_set_gradient(v, NULL);  // TODO

    int stop = 0;
    const uint32_t sleep_time = stream_sleep_time(s);
    while (!window_should_close(w) && !stop) {
        stop = stream_read(s);
        stream_rewind(s, -((int64_t)NOF_SAMPLES / 2));
        stream_transform(s);
        double* freqs = stream_frequencies(s);

        visualizer_set_heights(v, NOF_SHAPES, freqs);
        visualizer_draw(v);

        swap_buffers(w);
        poll_events();

        usleep(sleep_time);
    }

    ret = EXIT_SUCCESS;

cleanup:
    visualizer_free(&v);
    stream_free(&s);
    window_free(w);

    return ret;
}