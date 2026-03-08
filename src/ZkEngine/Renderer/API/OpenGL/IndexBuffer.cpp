#include "IndexBuffer.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

IndexBuffer::IndexBuffer(std::span<const std::uint32_t> data) : m_Count(data.size()) {
    assert(sizeof(uint32_t) == sizeof(GLuint));

    // glGenerateBuffers(1, &m_RendererID);
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}
void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
