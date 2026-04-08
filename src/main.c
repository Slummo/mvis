#include <mvis/track.h>
#include <mvis/window.h>
#include <mvis/render.h>

#include <stdint.h>
#include <stdlib.h>

#define WIDTH (640)
#define HEIGHT (400)
#define TITLE ("MVIS")

int32_t main(int32_t argc, char** argv) {
    int32_t ret = EXIT_FAILURE;

    track* track = NULL;
    GLFWwindow* window = NULL;

    track = track_new(argv[1]);
    if (!track) {
        goto cleanup;
    }

    window = window_new(WIDTH, HEIGHT, TITLE);
    if (!window) {
        goto cleanup;
    }

    render_ctx ctx = {0};
    if (render_init(&ctx) != 0) {
        goto cleanup;
    }

    int32_t stop = 0;
    while (!window_should_close(window) && !stop) {
        stop = draw_track(&ctx, track, 0.0f, 0.0f, 2.0f, 2.0f);
        window_swap_buffers(window);
        poll_events();
    }

    ret = EXIT_SUCCESS;

cleanup:
    window_free(window);
    track_free(&track);
    render_ctx_free(&ctx);

    return ret;
}