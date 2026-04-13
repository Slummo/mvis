#include <mvis/engine.h>
#include <mvis/log.h>
#include <stdlib.h>

#define WIN_LEN (1ull << 10)

int main(int argc, char* argv[]) {
    int ret = EXIT_FAILURE;

    engine* engine = engine_new();
    if (!engine) {
        ERR("engine_new failed");
        goto cleanup;
    }

    track_id t1 = engine_add(engine, "song.wav", WIN_LEN);
    if (t1 == -1) {
        ERR("engine_add failed");
        goto cleanup;
    }

    if (engine_start(engine, t1) != 0) {
        ERR("engine_start failed");
        goto cleanup;
    }

    ret = EXIT_SUCCESS;

cleanup:
    engine_free(engine);
    return ret;
}