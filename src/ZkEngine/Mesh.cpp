#include "Mesh.hpp"

#include "VertexBufferLayout.hpp"

namespace zk {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) :
    m_Vertices(std::move(vertices)), m_Indices(std::move(indices)) {
    vao.bind();
    vbo = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

    layout.push<float>(3);  // position
    layout.push<float>(3);  // normal
    layout.push<float>(2);  // uv
    vao.addBuffer(*vbo, layout);

    ibo = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
    ibo->bind();

    vao.unbind();
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) {
    vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(glm::vec3));

    VertexBufferLayout layout;
    layout.push<float>(3);  // position
    vao.addBuffer(*vbo, layout);

    ibo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
    ibo->bind();

    vao.unbind();
}

// Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
//     vao.bind();
//     vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
//
//     VertexBufferLayout layout;
//     layout.push<float>(3);  // position
//     layout.push<float>(3);  // normal
//     layout.push<float>(2);  // uv
//     vao.addBuffer(*vbo, layout);
//
//     ibo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
//
//     vao.unbind();
// }

void Mesh::bind() const {
    vao.bind();
}

void Mesh::unbind() const {
    vao.unbind();
    vbo->unbind();
    ibo->unbind();
}

IndexBuffer& Mesh::getIndexBuffer() const {
    return *ibo;
}

void Mesh::updateVertices(const glm::vec3& p1, const glm::vec3& p2) {
    m_Vertices[0].position = p1;
    m_Vertices[1].position = p2;

    vao.bind();
    vbo = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

    vao.addBuffer(*vbo, layout);

    ibo = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
    ibo->bind();

    vao.unbind();
}

}
