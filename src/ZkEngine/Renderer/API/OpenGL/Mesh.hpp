#pragma once

#include <memory>
#include <vector>
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Vertex.hpp"
#include "VertexBufferLayout.hpp"
#include <glm/glm.hpp>

namespace zk {

class Mesh {
   public:
    Mesh() = delete;
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    // Mesh(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);
    // Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    void bind() const;
    void unbind() const;

    IndexBuffer& getIndexBuffer() const;

    void setTriangle(bool isTriangle) { m_IsTriangle = isTriangle; }
    bool isTriangle() const { return m_IsTriangle; }

    void updateVertices(const glm::vec3& p1, const glm::vec3& p2);

   private:
    VertexArray vao;
    std::unique_ptr<VertexBuffer> vbo;
    std::unique_ptr<IndexBuffer> ibo;

    VertexBufferLayout layout;

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;

    bool m_IsTriangle = true;
};

}
