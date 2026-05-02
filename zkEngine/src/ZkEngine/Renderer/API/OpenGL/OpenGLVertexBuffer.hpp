#pragma once

#include "ZkEngine/Renderer/VertexBuffer.hpp"

#include <glad/glad.h>
#include <span>

namespace zk {

class Vertex;

enum class VertexBufferUsage : uint32_t {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
};

static constexpr uint32_t ToGL(VertexBufferUsage usage) {
    return static_cast<uint32_t>(usage);
}

class OpenGLVertexBuffer : public VertexBuffer {
  public:
    OpenGLVertexBuffer(std::span<const Vertex> data);
    OpenGLVertexBuffer(
        std::span<const std::byte> data, VertexBufferUsage usage = VertexBufferUsage::Static);

    ~OpenGLVertexBuffer();

    OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
    OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;

    OpenGLVertexBuffer(OpenGLVertexBuffer&&) noexcept = default;
    OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&&) noexcept = default;

    void bind() const override;
    void unbind() const override;

    void update(std::span<const std::byte> data) override;

  private:
    uint32_t m_RendererID = 0;

    uint32_t m_Usage = GL_STATIC_DRAW;
    size_t m_Size = 0;
};

} // namespace zk
