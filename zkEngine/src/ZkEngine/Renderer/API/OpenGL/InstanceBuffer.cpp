#include "zkpch.hpp"
#include "InstanceBuffer.hpp"

#include <glad/glad.h>

namespace zk {

InstanceBuffer::InstanceBuffer(size_t size) : m_Size(size) {
    glCreateBuffers(1, &m_RendererID);
    glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
}

InstanceBuffer::~InstanceBuffer() {
    if (m_RendererID) {
        glDeleteBuffers(1, &m_RendererID);
    }
}

void InstanceBuffer::update(const void* data, size_t size, size_t offset) {
    // ZK_CORE_ASSERT(offset + size <= m_Size, "InstanceBuffer overflow!");

    // Resize if needed
    if (offset + size > m_Size) {
        glNamedBufferData(m_RendererID, offset + size, data, GL_DYNAMIC_DRAW);
        m_Size = offset + size;
    }
    else {
        glNamedBufferSubData(m_RendererID, offset, size, data);
    }
}

void InstanceBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void InstanceBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace zk
