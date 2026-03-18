#pragma once

#include "glm/fwd.hpp"
#include <filesystem>
#include <vector>

namespace zk {

// This is the API that the application will use
class Renderer {
  public:
    Renderer() = delete;

    static void init();
    static void update();
    static void beginFrame();
    // static void submit(const RenderObject& obj, const Camera3D& camera);
    static void endFrame();

    static void setClearColor(const glm::vec4& color);
    static void debug();

    static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    static void drawLine(const std::vector<glm::vec3>& pts);

    static void setSkybox(const std::vector<std::filesystem::path>& tex);

};

} // namespace zk
