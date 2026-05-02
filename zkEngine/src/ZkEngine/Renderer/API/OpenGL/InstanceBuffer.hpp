#pragma once

namespace zk {

class InstanceBuffer {
  public:
    InstanceBuffer(size_t size);
    ~InstanceBuffer();

    void update(const void* data, size_t size, size_t offset = 0);
    void bind() const;
    void unbind() const;

  private:
    uint32_t m_RendererID = 0;
    size_t m_Size = 0;
};

} // namespace zk
