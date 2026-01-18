#include "Window.hpp"
#include <GLFW/glfw3.h>

#include "ZkEngine/Input/GLFWInput.hpp"

void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(const WindowSpecs& specs) : m_Specs(specs) {}

Window::~Window() {
    destroy();
    glfwTerminate();
}

void Window::create() {
    glfwSetErrorCallback(glfw_error_callback);

    glfwInit();

    // This setups OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // important for debug callback

    // MSAA
    glfwWindowHint(GLFW_SAMPLES, 8);

    m_Window = glfwCreateWindow(m_Specs.width, m_Specs.height, m_Specs.title.c_str(), NULL, NULL);
    if (!m_Window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /* Make the m_Window's context current */
    glfwMakeContextCurrent(m_Window);

    gladLoadGL();

    // Vsync
    glfwSwapInterval(m_Specs.vsync ? 1 : 0);

    // Initial call to set viewport
    glViewport(0, 0, m_Specs.width, m_Specs.height);

    glfwSetFramebufferSizeCallback(m_Window, frameBufferSizeCallback);

    GLFWInput::RegisterCallbacks(m_Window);
}

void Window::destroy() {
    if (m_Window)
        glfwDestroyWindow(m_Window);

    m_Window = nullptr;
}

void Window::update() {
    glfwSwapBuffers(m_Window);
}

glm::vec2 Window::getSize() const {
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);

    return glm::vec2(width, height);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_Window) != 0;
}

void Window::setVsync(bool vsync) {
    m_Specs.vsync = vsync;
    glfwSwapInterval(m_Specs.vsync ? 1 : 0);
}

bool Window::isCursorLocked() const {
    if (glfwGetInputMode(m_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        return true;
    }
    return false;
}

// Maybe use cursor mode struct instead of "cursor locked"
void Window::setCursorLocked(bool locked) {
    if (locked) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

int Window::getCursorMode() const {
    return glfwGetInputMode(m_Window, GLFW_CURSOR);
}
