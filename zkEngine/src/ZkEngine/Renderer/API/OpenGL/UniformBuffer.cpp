#include "zkpch.hpp"
#include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"
#include <glad/glad.h>

namespace zk {

UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding) {
    glCreateBuffers(1, &m_RendererID);
    glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &m_RendererID);
}

void UniformBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
    glNamedBufferSubData(m_RendererID, offset, size, data);
}

} // namespace zk
