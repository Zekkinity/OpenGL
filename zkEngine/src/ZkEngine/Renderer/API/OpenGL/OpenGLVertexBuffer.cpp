#include "zkpch.hpp"
#include "OpenGLVertexBuffer.hpp"

#include <glad/glad.h>

#include "ZkEngine/Renderer/Vertex.hpp"

namespace zk {

OpenGLVertexBuffer::OpenGLVertexBuffer(std::span<const Vertex> data) {
    // VBO
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(std::span<const std::byte> data, VertexBufferUsage usage)
    : m_Size(data.size_bytes()), m_Usage(ToGL(usage)) {
    glCreateBuffers(1, &m_RendererID);
    glNamedBufferData(m_RendererID, m_Size, data.data(), m_Usage);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    if (m_RendererID) {
        glDeleteBuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
}

void OpenGLVertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}
void OpenGLVertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::update(std::span<const std::byte> data) {
    if (data.size_bytes() > m_Size) {
        glNamedBufferData(m_RendererID, data.size_bytes(), data.data(), m_Usage);
        m_Size = data.size_bytes();
    }
    else {
        glNamedBufferSubData(m_RendererID, 0, data.size_bytes(), data.data());
    }
}

} // namespace zk
