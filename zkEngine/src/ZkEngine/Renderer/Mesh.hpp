#pragma once

#include "ZkEngine/Renderer/IndexBuffer.hpp"

#include <glm/glm.hpp>

namespace zk {

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB transform(const glm::mat4& model) const {
        glm::vec3 corners[8] = {
            {min.x, min.y, min.z},
            {max.x, min.y, min.z},
            {min.x, max.y, min.z},
            {max.x, max.y, min.z},
            {min.x, min.y, max.z},
            {max.x, min.y, max.z},
            {min.x, max.y, max.z},
            {max.x, max.y, max.z},
        };

        glm::vec3 newMin(FLT_MAX);
        glm::vec3 newMax(-FLT_MAX);

        for (int i = 0; i < 8; i++) {
            glm::vec3 world = glm::vec3(model * glm::vec4(corners[i], 1.0));
            newMin = glm::min(newMin, world);
            newMax = glm::max(newMax, world);
        }

        return {newMin, newMax};
    }
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
