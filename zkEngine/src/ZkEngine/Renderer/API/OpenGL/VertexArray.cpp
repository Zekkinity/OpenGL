#include "zkpch.hpp"

#include "ZkEngine/Renderer/VertexBuffer.hpp"
#include "VertexArray.hpp"

#include <glad/glad.h>
#include "VertexBufferLayout.hpp"

namespace zk {

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_RendererID(other.m_RendererID) {
    other.m_RendererID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (m_RendererID != 0)
            glDeleteVertexArrays(1, &m_RendererID);

        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }
    return *this;
}

void VertexArray::addBuffer(const VertexBuffer& vbo, const VertexBufferLayout& layout) {
    bind();

    vbo.bind();

    const auto& elements = layout.getElements();
    // unsigned int offset = 0;
    intptr_t offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];

        // This lets us specify the layout of the data in the buffer.
        // also links the current binded buffer to the VAO
        glVertexAttribPointer(
            i,
            element.count,
            element.type,
            element.normalized,
            layout.getStride(),
            (const void*)offset);
        glEnableVertexAttribArray(i);

        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}

void VertexArray::addInstancedMat4(uint32_t startLocation) {
    bind();

    for (uint32_t i = 0; i < 4; i++) {
        glEnableVertexAttribArray(startLocation + i);

        glVertexAttribPointer(
            startLocation + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::mat4),
            (const void*)(sizeof(float) * i * 4));

        glVertexAttribDivisor(startLocation + i, 1);
    }
}

void VertexArray::bind() const {
    assert(m_RendererID != 0);
    glBindVertexArray(m_RendererID);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

} // namespace zk
