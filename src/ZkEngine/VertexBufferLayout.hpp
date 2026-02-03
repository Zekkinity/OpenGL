#pragma once

#include <cassert>
#include <vector>
#include <glad/glad.h>

namespace zk {

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type) {
        switch (type) {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexBufferLayout {
  public:
    VertexBufferLayout() : m_Stride(0) {}

    template <typename T>
    void push(unsigned int count) {
        static_assert(false);
    }

    const std::vector<VertexBufferElement>& getElements() const { return m_Elements; }

    unsigned int getStride() const { return m_Stride; }

  private:
    std::vector<VertexBufferElement> m_Elements;

    unsigned int m_Stride;
};

template <>
inline void VertexBufferLayout::push<float>(unsigned int count) {
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
}
template <>
inline void VertexBufferLayout::push<unsigned int>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
}

template <>
inline void VertexBufferLayout::push<unsigned char>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
}

} // namespace zk
