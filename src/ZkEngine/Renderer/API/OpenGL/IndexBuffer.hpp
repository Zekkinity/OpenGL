#pragma once

#include <cstdint>
#include <span>

class IndexBuffer {
   public:
    IndexBuffer(std::span<const std::uint32_t>);

    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    unsigned int getCount() const { return m_Count; }

   private:
    unsigned int m_RendererID;  // IBO
    unsigned int m_Count;
};
