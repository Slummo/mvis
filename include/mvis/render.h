#pragma once

#include <stddef.h>

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

typedef struct visualizer visualizer;

visualizer* visualizer_new(size_t nof_instances);
void visualizer_set_background(visualizer* visualizer, col4f background);
void visualizer_set_shape(visualizer* visualizer, shape2D s);
void visualizer_set_gradient(visualizer* visualizer, color_gradient* gradient);
void visualizer_set_heights(visualizer* visualizer, size_t n, const double heights[static n]);
void visualizer_draw(const visualizer* visualizer);
void visualizer_free(visualizer** visualizer_ptr);