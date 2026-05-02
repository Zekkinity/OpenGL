#pragma once

#include <span>

namespace zk {

class Vertex;

class VertexBuffer {
  public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() = default;

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&&) noexcept = default;
    VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void update(std::span<const std::byte> data) = 0;
};

} // namespace zk
