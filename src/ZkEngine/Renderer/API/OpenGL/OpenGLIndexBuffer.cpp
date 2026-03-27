#include "OpenGLIndexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

OpenGLIndexBuffer::OpenGLIndexBuffer(std::span<const std::uint32_t> data) : m_Count(data.size()) {
    assert(sizeof(uint32_t) == sizeof(GLuint));

    // glGenerateBuffers(1, &m_RendererID);
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGLIndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}
void OpenGLIndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
