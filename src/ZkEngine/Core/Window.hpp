#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <cstdint>
#include <string>

struct WindowSpecs {
    std::string title;
    uint32_t width = 1280;
    uint32_t height = 720;
    bool vsync = true;
};

class Window {
   public:
    Window(const WindowSpecs& specs);
    ~Window();

    void create();
    void destroy();

    void update();
    bool shouldClose() const;
    void setVsync(bool vsync);

    bool isCursorLocked() const;
    void setCursorLocked(bool locked);
    int getCursorMode() const;

    glm::vec2 getSize() const;
    GLFWwindow* getWindow() const { return m_Window; }

    int getWidth() const { return m_Specs.width; }
    int getHeight() const { return m_Specs.height; }

    void setWidth(int width) { m_Specs.width = width; }
    void setHeight(int height) { m_Specs.height = height; }

   private:
    GLFWwindow* m_Window;

    WindowSpecs m_Specs;
};
