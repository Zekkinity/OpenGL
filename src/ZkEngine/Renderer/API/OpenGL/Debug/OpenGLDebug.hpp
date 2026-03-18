#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class OpenGLDebug {
  public:
    static void init(bool enabled);

  private:
    static void APIENTRY callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam);
};

} // namespace zk
