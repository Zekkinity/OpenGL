#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "EngineSpecs.hpp"

namespace zk {

class Window {
  public:
    Window(const EngineSpecs& specs);
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

    void makeContextCurrent() { glfwMakeContextCurrent(m_Window); }

  private:
    GLFWwindow* m_Window;

    EngineSpecs m_Specs;
};

} // namespace zk
