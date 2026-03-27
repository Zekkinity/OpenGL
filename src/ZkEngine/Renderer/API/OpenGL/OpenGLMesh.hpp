#pragma once

#include "ZkEngine/Renderer/Mesh.hpp"
#include "ZkEngine/Renderer/Vertex.hpp"

#include "ZkEngine/Renderer/IndexBuffer.hpp"
#include "VertexArray.hpp"

#include <memory>
#include <vector>

#include "VertexBufferLayout.hpp"
#include <glm/glm.hpp>

namespace zk {

class OpenGLMesh : public Mesh {
  public:
    OpenGLMesh() = delete;
    OpenGLMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    void bind() const override;
    void unbind() const override;

    IndexBuffer& getIndexBuffer() const override;

    void setTriangle(bool isTriangle) override { m_IsTriangle = isTriangle; }
    bool isTriangle() const override { return m_IsTriangle; }

    void updateVertices(const glm::vec3& p1, const glm::vec3& p2) override;

  private:
    VertexArray vao;
    std::unique_ptr<VertexBuffer> vbo;
    std::unique_ptr<IndexBuffer> ibo;

    VertexBufferLayout layout;

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;

    bool m_IsTriangle = true;
};

} // namespace zk
