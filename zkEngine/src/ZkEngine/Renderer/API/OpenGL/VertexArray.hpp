#pragma once

namespace zk {

class VertexBuffer;
class VertexBufferLayout;

class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void addInstancedMat4(uint32_t startLocation = 3);

    void bind() const;
    void unbind() const;

  private:
    uint32_t m_RendererID;
};

} // namespace zk
