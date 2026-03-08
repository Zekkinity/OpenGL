#pragma once

#include <memory>

namespace zk {
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

    static std::unique_ptr<Framebuffer> create();
};

} // namespace zk
