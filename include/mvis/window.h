#pragma once

#include <stdint.h>

typedef struct GLFWwindow GLFWwindow;

GLFWwindow* window_new(int32_t w, int32_t h, const char* title);
int32_t window_should_close(GLFWwindow* window);
void window_swap_buffers(GLFWwindow* window);
void window_free(GLFWwindow* window);
void poll_events(void);