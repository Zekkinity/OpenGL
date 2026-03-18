#pragma once

#include "ZkEngine/Renderer/Vertex.hpp"
#include <memory>
#include <span>

namespace zk {

class VertexBuffer {
  public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    static std::unique_ptr<VertexBuffer> create(std::span<const Vertex> data);
};

} // namespace zk
