#include "OpenGLVertexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

namespace zk {

OpenGLVertexBuffer::OpenGLVertexBuffer(std::span<const Vertex> data) {
    // VBO
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGLVertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}
void OpenGLVertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace zk
