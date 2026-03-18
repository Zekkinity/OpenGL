#pragma once

#include "ZkEngine/Renderer/Vertex.hpp"
#include "ZkEngine/Renderer/VertexBuffer.hpp"
#include <span>

namespace zk {

class OpenGLVertexBuffer : public VertexBuffer {
  public:
    OpenGLVertexBuffer(std::span<const Vertex> data);

    ~OpenGLVertexBuffer();

    void bind() const override;
    void unbind() const override;

  private:
    uint32_t m_RendererID;
};

} // namespace zk
