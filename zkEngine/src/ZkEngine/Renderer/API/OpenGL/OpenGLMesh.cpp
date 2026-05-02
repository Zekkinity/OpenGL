#include "zkpch.hpp"
#include "OpenGLMesh.hpp"

#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "VertexBufferLayout.hpp"

namespace zk {

OpenGLMesh::OpenGLMesh(RenderDevice& device, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)) {
    computeBounds();
    computeTangents(m_Vertices, m_Indices);

    vao.bind();
    vbo = device.createVertexBuffer(std::as_bytes(std::span(m_Vertices)));

    layout.push<float>(3); // position
    layout.push<float>(3); // normal
    layout.push<float>(2); // uv

    layout.push<float>(3); // tangent
    layout.push<float>(3); // bitangent

    vao.addBuffer(*vbo, layout);

    ibo = device.createIndexBuffer(m_Indices);
    ibo->bind();

    vao.unbind();
}

void OpenGLMesh::bind() const {
    vao.bind();
}

void OpenGLMesh::unbind() const {
    vao.unbind();
}

IndexBuffer& OpenGLMesh::getIndexBuffer() const {
    return *ibo;
}

void OpenGLMesh::updateVertices(const glm::vec3& p1, const glm::vec3& p2) {
    // m_Vertices[0].position = p1;
    // m_Vertices[1].position = p2;
    //
    // vao.bind();
    // vbo = Renderer::getDevice().createVertexBuffer(std::as_bytes(std::span(m_Vertices)));
    //
    // vao.addBuffer(*vbo, layout);
    //
    // ibo = Renderer::getDevice().createIndexBuffer(m_Indices);
    // ibo->bind();
    //
    // vao.unbind();
}

void OpenGLMesh::computeTangents(
    std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i + 0]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        glm::vec3 bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    // Normalize after accumulation
    for (auto& v : vertices) {
        v.tangent = glm::normalize(v.tangent);
        v.bitangent = glm::normalize(v.bitangent);
    }
}

void OpenGLMesh::computeBounds() {
    if (m_Vertices.empty())
        return;

    glm::vec3 min(FLT_MAX);
    glm::vec3 max(-FLT_MAX);

    for (const auto& vertex : m_Vertices) {
        min = glm::min(min, vertex.position);
        max = glm::max(max, vertex.position);
    }

    m_Bounds.min = min;
    m_Bounds.max = max;
}

} // namespace zk
