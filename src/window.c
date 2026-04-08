#include <mvis/window.h>
#include <mvis/log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

    glViewport(0, 0, w, h);

    return window;
}

int32_t window_should_close(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}

void window_swap_buffers(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

void window_free(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void poll_events(void) {
    return glfwPollEvents();
}