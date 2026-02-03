#pragma once

#include "glm/fwd.hpp"

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
};

} // namespace zk
