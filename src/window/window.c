#include "window.h"
#include <mvis/log.h>
#include <signal.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
                                      const GLchar* message, const void* /*user_param*/) {
    fprintf(stderr, "[GL_DEBUG] source=0x%x type=0x%x id=%u severity=0x%x\n\t%s\n", source, type, id, severity,
            message);

    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        raise(SIGTRAP);
    }
}

GLFWwindow* window_new(int32_t w, int32_t h, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        ERR("Error with glfwInit");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!window) {
        ERR("Error with glfwCreateWindow");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwShowWindow(window);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERR("Error with gladLoadGLLoader");
        window_free(window);
        return NULL;
    }

    // Set debug callback
    if (GLAD_GL_KHR_debug || GLAD_GL_VERSION_4_3) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    glViewport(0, 0, w, h);

    return window;
}

int32_t window_should_close(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}

void swap_buffers(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

void window_free(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void poll_events(void) {
    return glfwPollEvents();
}