#pragma once

#include "ZkEngine/Renderer/IndexBuffer.hpp"

#include <span>

class OpenGLIndexBuffer : public IndexBuffer {
   public:
    OpenGLIndexBuffer(std::span<const std::uint32_t>);

    ~OpenGLIndexBuffer();

    void bind() const override;
    void unbind() const override;

    uint32_t getCount() const override { return m_Count; }

   private:
    uint32_t m_RendererID;  // IBO
    uint32_t m_Count;
};
