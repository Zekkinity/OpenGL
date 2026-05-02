#pragma once

#include "ZkEngine/Renderer/IndexBuffer.hpp"

#include <glm/glm.hpp>

namespace zk {

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

class InstanceBuffer;

class Mesh {
  public:
    Mesh() = default;
    virtual ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual IndexBuffer& getIndexBuffer() const = 0;

    virtual void setTriangle(bool isTriangle) = 0;
    virtual bool isTriangle() const = 0;

    virtual void updateVertices(const glm::vec3& p1, const glm::vec3& p2) = 0;

    virtual void setInstanced(bool instanced) { m_Instanced = instanced; }
    virtual bool isInstanced() const { return m_Instanced; }

    virtual const AABB& getAABB() const = 0;

  private:
    bool m_Instanced = false;
};

} // namespace zk
