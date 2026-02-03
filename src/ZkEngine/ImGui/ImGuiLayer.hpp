#pragma once

#include <GLFW/glfw3.h>

namespace zk {

class ImGuiLayer {
  public:
    ImGuiLayer() = default;
    ~ImGuiLayer() = default;

    void init(GLFWwindow* window);
    void beginFrame();

    void endFrame();
    void shutdown();
};

} // namespace zk
