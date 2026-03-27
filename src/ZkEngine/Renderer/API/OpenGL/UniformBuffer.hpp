#pragma once

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

// NOTE:
// Make a UBO for this
// - bind 0: Camera
// - bind 1: Lights
// - bind 2: Materials

namespace zk {

class UniformBuffer {
  public:
    UniformBuffer(uint32_t size, uint32_t binding) {
        glGenBuffers(1, &m_RendererID);

        glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    ~UniformBuffer() { glDeleteBuffers(1, &m_RendererID); }

    void setData(const void* data, uint32_t size, uint32_t offset = 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        // OpenGL 4.5
        // glNamedBufferSubData(m_RendererID, offset, size, data);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

  private:
    uint32_t m_RendererID;
};

} // namespace zk
