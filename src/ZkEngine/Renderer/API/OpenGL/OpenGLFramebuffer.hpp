#pragma once

#include "ZkEngine/Renderer/Framebuffer.hpp"

namespace zk {

class OpenGLFramebuffer : public Framebuffer {
  public:
    OpenGLFramebuffer() = delete;
    OpenGLFramebuffer(const FramebufferSpec& specs);
    ~OpenGLFramebuffer();

    void bind() const override;
    void unbind() const override;

    uint32_t getColorAttachment(uint32_t index) const override;
    uint32_t getDepthAttachment() const override;

    uint32_t getWidth() const override { return m_Specs.width; }
    uint32_t getHeight() const override { return m_Specs.height; }

    void resize(uint32_t width, uint32_t height) override;

    void invalidate() override;

  private:
    void clear();

  private:
    uint32_t m_RendererID = 0;

    FramebufferSpec m_Specs;

    std::vector<FramebufferAttachmentSpec> m_ColorAttachmentsSpecs;
    FramebufferAttachmentSpec m_DepthAttachmentSpec;

    std::vector<uint32_t> m_ColorAttachments;
    uint32_t m_DepthAttachment = 0;

};

} // namespace zk
