#include "Mesh.hpp"

#include "VertexBufferLayout.hpp"

namespace zk {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) :
    m_Vertices(std::move(vertices)), m_Indices(std::move(indices)) {
    vao.bind();
    vbo = VertexBuffer::create(m_Vertices);

    layout.push<float>(3);  // position
    layout.push<float>(3);  // normal
    layout.push<float>(2);  // uv
    
    vao.addBuffer(*vbo, layout);

    ibo = std::make_unique<IndexBuffer>(m_Indices);
    ibo->bind();

    vao.unbind();
}

void Mesh::bind() const {
    vao.bind();
}

void Mesh::unbind() const {
    vao.unbind();
}

IndexBuffer& Mesh::getIndexBuffer() const {
    return *ibo;
}

void Mesh::updateVertices(const glm::vec3& p1, const glm::vec3& p2) {
    m_Vertices[0].position = p1;
    m_Vertices[1].position = p2;

    vao.bind();
    vbo = VertexBuffer::create(m_Vertices);

    vao.addBuffer(*vbo, layout);

    ibo = std::make_unique<IndexBuffer>(m_Indices);
    ibo->bind();

    vao.unbind();
}

}
