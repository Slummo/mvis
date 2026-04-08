#include <mvis/render.h>
#include <mvis/track.h>
#include <mvis/io.h>
#include <glad/glad.h>
#include <mvis/log.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

static int32_t compile_shader(GLenum type, const char* source, GLuint* shader) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, NULL);
    glCompileShader(sh);

    int32_t ret;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetShaderInfoLog(sh, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        return 1;
    }

    *shader = sh;

    return 0;
}

void render_ctx_free(render_ctx* ctx) {
    glDeleteVertexArrays(1, &ctx->vao);
    glDeleteBuffers(1, &ctx->pos_vbo);
    glDeleteBuffers(1, &ctx->ebo);
    glDeleteProgram(ctx->program);
}

static int32_t create_program(render_ctx* ctx) {
    char* vert_src = file_content("rect.vert");
    char* frag_src = file_content("rect.frag");

    GLuint vertex = 0;
    GLuint fragment = 0;

    if (compile_shader(GL_VERTEX_SHADER, vert_src, &vertex) != 0) {
        ERR("Error while compiling vertex shader");
        return 1;
    }

    if (compile_shader(GL_FRAGMENT_SHADER, frag_src, &fragment) != 0) {
        ERR("Error while compiling fragment shader");
        return 1;
    }

    free(vert_src);
    free(frag_src);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int32_t ret;
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetProgramInfoLog(program, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        return 1;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    ctx->program = program;

    return 0;
}

typedef struct vec3f {
    float x;
    float y;
    float z;
} vec3f;

typedef struct mat3f {
    vec3f v1;
    vec3f v2;
    vec3f v3;
} mat3f;

#define MAT3F_MODEL(scalex, scaley, transx, transy, skewx, skewy)                         \
    (mat3f) {                                                                             \
        {(float)(scalex), (float)(skewx), 0.0f}, {(float)(skewy), (float)(scaley), 0.0f}, \
            {(float)(transx), (float)(transy), 1.0f}                                      \
    }

typedef struct instance {
    mat3f model;
    vec3f color;
} instance;

#define NOF_RECTS (NOF_SAMPLES >> 1ul)
static instance instances[NOF_RECTS] = {0};

#define VERTEX_ATTRIB(index, size, stride, offset, divide, amount)                                         \
    do {                                                                                                   \
        glVertexAttribPointer(index, (size) / sizeof(float), GL_FLOAT, GL_FALSE, stride, (void*)(offset)); \
        glEnableVertexAttribArray(index);                                                                  \
        if (divide) {                                                                                      \
            glVertexAttribDivisor(index, amount);                                                          \
        }                                                                                                  \
    } while (0)

static int32_t create_buffers(render_ctx* ctx) {
    GLuint vao = 0;
    GLuint pos_vbo = 0;
    GLuint ebo = 0;
    GLuint inst_vbo = 0;

    // Generate objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &pos_vbo);
    glGenBuffers(1, &inst_vbo);
    glGenBuffers(1, &ebo);

    // VAO
    glBindVertexArray(vao);

    static const vec3f positions[4] = {
        {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}};
    static const int32_t indices[6] = {0, 1, 2, 0, 2, 3};

    // Position VBO
    glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);
    VERTEX_ATTRIB(0, sizeof(vec3f), sizeof(vec3f), 0, 0, 0);  // pos

    // Instance VBO
    glBindBuffer(GL_ARRAY_BUFFER, inst_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instances), NULL, GL_DYNAMIC_DRAW);  // Uninitialized

    VERTEX_ATTRIB(1, sizeof(vec3f), sizeof(instance), sizeof(vec3f) * 0, 1, 1);  // model vec1
    VERTEX_ATTRIB(2, sizeof(vec3f), sizeof(instance), sizeof(vec3f) * 1, 1, 1);  // model vec2
    VERTEX_ATTRIB(3, sizeof(vec3f), sizeof(instance), sizeof(vec3f) * 2, 1, 1);  // model vec3
    VERTEX_ATTRIB(4, sizeof(vec3f), sizeof(instance), sizeof(vec3f) * 3, 1, 1);  // color

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    ctx->vao = vao;
    ctx->pos_vbo = pos_vbo;
    ctx->ebo = ebo;
    ctx->inst_vbo = inst_vbo;

    return 0;
}

int32_t render_init(render_ctx* ctx) {
    if (!ctx) {
        return 1;
    }
    memset(ctx, 0, sizeof(render_ctx));

    if (create_program(ctx) != 0) {
        ERR("Error in create_program");
        return 1;
    }

    if (create_buffers(ctx) != 0) {
        ERR("Error in create_buffers");
        return 1;
    }

    glUseProgram(ctx->program);
    glBindVertexArray(ctx->vao);

    return 0;
}

int32_t draw_track(render_ctx* ctx, track* track, float offx, float offy, float width, float height) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double complex* freqs = track_read(track);
    if (!freqs) {
        return 1;
    }

    const double fs = track->info.samplerate;
    const useconds_t usleep_time = NOF_SAMPLES * 1000000 / fs;
    static const double norm_factor = 2.0 / NOF_SAMPLES;

    const float dx = width / NOF_RECTS;
    static const float db_floor = -60.0f;

    for (uint64_t i = 0; i < NOF_RECTS; ++i) {
        double m = norm_factor * cabs(freqs[i]);
        if (m < 0.001) {
            m = 0.001;
        }

        double db = 20.0 * log10(m);

        float recth = (db - db_floor) / (-db_floor);
        if (recth < 0.0f) {
            recth = 0.0f;
        }

        instances[i].model = MAT3F_MODEL(dx, recth * height, dx * i + offx - 1.0f, offy - 1.0f, 0.0f, 0.0f);
        instances[i].color = (vec3f){1.0f - m, 0.0f, m};
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance) * NOF_RECTS, instances);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, NOF_RECTS);

    free(freqs);
    usleep(usleep_time);

    return 0;
}
