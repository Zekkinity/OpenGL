#pragma once

#include "ZkEngine/Renderer/API/OpenGL/Vertex.hpp"
#include <span>

namespace zk {

class VertexBuffer {
  public:
    VertexBuffer(std::span<const Vertex> data);

    ~VertexBuffer();

    void bind() const;
    void unbind() const;

  private:
    unsigned int m_RendererID;
};

} // namespace zk
