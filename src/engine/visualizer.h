#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct vec2f {
    float x;
    float y;
} vec2f;

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

static inline mat3f mat3f_model(vec2f scale, vec2f transform, vec2f skew) {
    return (mat3f){{scale.x, skew.x, 0.0f}, {skew.y, scale.y, 0.0f}, {transform.x, transform.y, 1.0f}};
}

typedef struct instance_data {
    mat3f model;
    vec3f color;
} instance_data;

typedef enum shape2D { SHAPE_RECTANGLE = 0, SHAPE_OVAL, NOF_SHAPES } shape2D;

typedef struct col4f {
    float r;
    float g;
    float b;
    float a;
} col4f;

typedef struct color_gradient {
    size_t n;
    col4f* colors;
    float* percentages;
} color_gradient;

typedef uint32_t GLuint;

typedef enum program { PROG_RECT = 0, PROG_OVAL, NOF_PROGRAMS } program;
typedef enum vertex_array { VAO = 0, NOF_VERTEX_ARRAYS } vertex_array;
typedef enum buffer { RECT_VBO = 0, RECT_EBO, INST_VBO, NOF_BUFFERS } buffer;

typedef struct visualizer {
    GLuint programs[NOF_PROGRAMS];
    GLuint vertex_arrays[NOF_VERTEX_ARRAYS];
    GLuint buffers[NOF_BUFFERS];
    col4f background;
    color_gradient* gradient;
    size_t nof_instance_datas;
    instance_data* instance_datas;
} visualizer;

int visualizer_init(visualizer* visualizer, size_t nof_instance_datas);
void visualizer_set_background(visualizer* visualizer, col4f background);
void visualizer_set_shape(visualizer* visualizer, shape2D s);
void visualizer_set_gradient(visualizer* visualizer, color_gradient* gradient);
void visualizer_set_heights(visualizer* visualizer, size_t n, const float heights[static n]);
void visualizer_draw(const visualizer* visualizer);
void visualizer_cleanup(visualizer* visualizer);