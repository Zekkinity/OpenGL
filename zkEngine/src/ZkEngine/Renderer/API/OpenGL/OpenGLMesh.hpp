#pragma once

#include "ZkEngine/Renderer/Mesh.hpp"

#include "ZkEngine/Renderer/Vertex.hpp"
#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/IndexBuffer.hpp"
#include "VertexArray.hpp"

#include <vector>

#include "VertexBufferLayout.hpp"
#include <glm/glm.hpp>

namespace zk {

class RenderDevice;

class OpenGLMesh : public Mesh {
  public:
    OpenGLMesh() = delete;
    OpenGLMesh(RenderDevice& device, std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void bind() const override;
    void unbind() const override;

    IndexBuffer& getIndexBuffer() const override;

    void setTriangle(bool isTriangle) override { m_IsTriangle = isTriangle; }
    bool isTriangle() const override { return m_IsTriangle; }

    void updateVertices(const glm::vec3& p1, const glm::vec3& p2) override;

    virtual const AABB& getAABB() const override { return m_Bounds; }

  private:
    void computeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    void computeBounds();

  private:
    VertexArray vao;
    Scope<VertexBuffer> vbo;
    Scope<IndexBuffer> ibo;

    VertexBufferLayout layout;

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;

    bool m_IsTriangle = true;

    AABB m_Bounds;
};

} // namespace zk
