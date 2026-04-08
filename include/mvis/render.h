#pragma once

#include <stdint.h>

typedef struct track track;

typedef struct render_ctx {
    uint32_t program;
    uint32_t vao;
    uint32_t pos_vbo;
    uint32_t ebo;
    uint32_t inst_vbo;
} render_ctx;

void render_ctx_free(render_ctx* ctx);

int32_t render_init(render_ctx* ctx);
int32_t draw_track(render_ctx* ctx, track* track, float offx, float offy, float width, float height);