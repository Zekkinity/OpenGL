#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

#include <vector>

namespace zk {

struct FramebufferAttachmentSpec {
    TextureFormat format = TextureFormat::RGB8;
    TextureType type = TextureType::Texture2D;
    TextureFilter filter = TextureFilter::Linear;
};

struct FramebufferSpec {
    uint32_t width = 0;
    uint32_t height = 0;

    std::vector<FramebufferAttachmentSpec> attachments;
};

class Framebuffer {
  protected:
    Framebuffer() = default;

  public:
    virtual ~Framebuffer() = default;

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;
    virtual uint32_t getID() const = 0;

    virtual const std::vector<FramebufferAttachmentSpec>& getColorAttachments() const = 0;
    virtual uint32_t getColorAttachment(uint32_t index) const = 0;
    virtual uint32_t getDepthAttachment() const = 0;
    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual void invalidate() = 0;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
};

} // namespace zk
