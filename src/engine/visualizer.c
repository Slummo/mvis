#include "visualizer.h"
#include "io/io.h"
#include <glad/glad.h>
#include <mvis/log.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &source, NULL);
    glCompileShader(s);

    int ret;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        return 0;
    }

    return s;
}

static GLuint create_program(const char* vert_filename, const char* frag_filename) {
    char* vert_src = file_content(vert_filename);
    char* frag_src = file_content(frag_filename);

    if (!vert_src || !frag_src) {
        free(vert_src);
        free(frag_src);
        return 0;
    }

    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vert_src);
    if (!vertex) {
        ERR("Error while compiling vertex shader");
        free(vert_src);
        free(frag_src);
        return 0;
    }

    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!fragment) {
        ERR("Error while compiling fragment shader");
        free(vert_src);
        free(frag_src);
        return 0;
    }

    free(vert_src);
    free(frag_src);

    GLuint p = glCreateProgram();
    glAttachShader(p, vertex);
    glAttachShader(p, fragment);
    glLinkProgram(p);

    int ret;
    glGetProgramiv(p, GL_LINK_STATUS, &ret);
    if (!ret) {
        char buf[1024];
        glGetProgramInfoLog(p, 1024, NULL, buf);
        fprintf(stderr, "%s", buf);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(p);
        return 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return p;
}

static void create_programs(visualizer* v) {
    GLuint rect_prog = create_program("rect.vert", "rect.frag");
    if (rect_prog == 0) {
        ERR("Invalid program");
        exit(EXIT_FAILURE);
    }
    v->programs[PROG_RECT] = rect_prog;
    // TODO create others
}

#define VERTEX_ATTRIB(index, size, stride, offset, divide, amount)                                         \
    do {                                                                                                   \
        glVertexAttribPointer(index, (size) / sizeof(float), GL_FLOAT, GL_FALSE, stride, (void*)(offset)); \
        glEnableVertexAttribArray(index);                                                                  \
        if (divide) {                                                                                      \
            glVertexAttribDivisor(index, amount);                                                          \
        }                                                                                                  \
    } while (0)

static void create_vertex_arrays(visualizer* v) {
    glGenVertexArrays(NOF_VERTEX_ARRAYS, v->vertex_arrays);
    glBindVertexArray(v->vertex_arrays[VAO]);
}

static void create_buffers(visualizer* v) {
    glGenBuffers(NOF_BUFFERS, v->buffers);

    static const vec2f rect_positions[4] = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};
    static const int32_t rect_indices[6] = {0, 1, 2, 0, 2, 3};

    // Rectangle VBO
    glBindBuffer(GL_ARRAY_BUFFER, v->buffers[RECT_VBO]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_positions), rect_positions, GL_STATIC_DRAW);
    VERTEX_ATTRIB(0, sizeof(vec2f), sizeof(vec2f), 0, 0, 0);  // pos

    // Rectangle EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->buffers[RECT_EBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices, GL_STATIC_DRAW);

    // instance_data VBO
    glBindBuffer(GL_ARRAY_BUFFER, v->buffers[INST_VBO]);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(instance_data) * v->nof_instance_datas), NULL,
                 GL_DYNAMIC_DRAW);  // Uninitialized

    VERTEX_ATTRIB(1, sizeof(vec3f), sizeof(instance_data), sizeof(vec3f) * 0, 1, 1);  // model vec1
    VERTEX_ATTRIB(2, sizeof(vec3f), sizeof(instance_data), sizeof(vec3f) * 1, 1, 1);  // model vec2
    VERTEX_ATTRIB(3, sizeof(vec3f), sizeof(instance_data), sizeof(vec3f) * 2, 1, 1);  // model vec3
    VERTEX_ATTRIB(4, sizeof(vec3f), sizeof(instance_data), sizeof(vec3f) * 3, 1, 1);  // color
}

int visualizer_init(visualizer* visualizer, size_t nof_instance_datas) {
    if (!visualizer) {
        return 1;
    }

    instance_data* instance_datas = calloc(nof_instance_datas, sizeof(instance_data));
    if (!instance_datas) {
        ERR("OOM");
        return 1;
    }

    visualizer->nof_instance_datas = nof_instance_datas;
    visualizer->instance_datas = instance_datas;

    create_programs(visualizer);
    create_vertex_arrays(visualizer);
    create_buffers(visualizer);

    return 0;
}

void visualizer_set_background(visualizer* visualizer, col4f background) {
    assert(visualizer);
    visualizer->background = background;
}

void visualizer_set_shape(visualizer* visualizer, shape2D s) {
    assert(visualizer);
    if (s == SHAPE_OVAL) {
        ERR("Shape not yet implemented");
        exit(EXIT_FAILURE);
    }

    glUseProgram(visualizer->programs[PROG_RECT]);
}

void visualizer_set_gradient(visualizer* visualizer, color_gradient* gradient) {
    assert(visualizer);
    visualizer->gradient = gradient;
}

void visualizer_set_heights(visualizer* visualizer, size_t n, const float heights[static n]) {
    assert(visualizer && n >= visualizer->nof_instance_datas);

    const size_t draw_count = visualizer->nof_instance_datas;
    const float dx = 2.0f / draw_count;  // X spans from -1.0 to 1.0
    const float bin_min = 1.0;
    const float bin_max = n;

    size_t last_bin_end = 0;
    for (size_t i = 0; i < draw_count; ++i) {
        // Normalized between 0.0 and 1.0
        float t = (float)i / draw_count;
        float t_next = (float)(i + 1) / draw_count;

        // Exponential to map t to a bin
        size_t bin_start = (size_t)(bin_min * powf(bin_max / bin_min, t));
        size_t bin_end = (size_t)(bin_min * powf(bin_max / bin_min, t_next));

        // Start from the previous bin
        if (bin_start < last_bin_end) {
            bin_start = last_bin_end;
        }

        // Every bin spans at least one index
        if (bin_end <= bin_start) {
            bin_end = bin_start + 1;
        }

        // Avoid OOB
        if (bin_end > n) {
            bin_end = n;
        }

        last_bin_end = bin_end;

        // Arithemtic mean
        float sum = 0.0;
        for (size_t j = bin_start; j < bin_end; ++j) {
            sum += heights[j];
        }

        float h = (float)(sum / (bin_end - bin_start));
        vec2f c = {-1.0f + dx * i + dx / 2.0f, -1.0f + h / 2.0f};
        visualizer->instance_datas[i].model = mat3f_model((vec2f){dx, h}, c, (vec2f){0});
        visualizer->instance_datas[i].color = (vec3f){h, 1.0f - h, 0.0f};
    }

    glBindBuffer(GL_ARRAY_BUFFER, visualizer->buffers[INST_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(instance_data) * draw_count), visualizer->instance_datas);
}

void visualizer_draw(const visualizer* visualizer) {
    col4f b = visualizer->background;
    glClearColor(b.r, b.g, b.b, b.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(visualizer->vertex_arrays[VAO]);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, visualizer->nof_instance_datas);
}

void visualizer_cleanup(visualizer* visualizer) {
    if (!visualizer) {
        return;
    }

    for (int i = 0; i < NOF_PROGRAMS; ++i) {
        glDeleteProgram(visualizer->programs[i]);
    }
    glDeleteVertexArrays(NOF_VERTEX_ARRAYS, visualizer->vertex_arrays);
    glDeleteBuffers(NOF_BUFFERS, visualizer->buffers);

    free(visualizer->instance_datas);

    memset(visualizer, 0, sizeof(struct visualizer));
}
