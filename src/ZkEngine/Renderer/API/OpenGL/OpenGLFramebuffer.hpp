#pragma once

#include "ZkEngine/Renderer/Framebuffer.hpp"

namespace zk {

class OpenGLFramebuffer : public Framebuffer {
  public:
    static std::unique_ptr<OpenGLFramebuffer> create();
    ~OpenGLFramebuffer();

    void bind() const override;
    void unbind() const override;

  private:
    OpenGLFramebuffer() = delete;
    OpenGLFramebuffer(uint32_t id);

  private:
    uint32_t m_RendererID = 0;
};

} // namespace zk
