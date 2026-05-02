#pragma once

// NOTE:
// Make a UBO for this
// - bind 0: Camera
// - bind 1: Lights
// - bind 2: Materials

namespace zk {

class UniformBuffer {
  public:
    UniformBuffer(uint32_t size, uint32_t binding);
    ~UniformBuffer();

    void setData(const void* data, uint32_t size, uint32_t offset = 0);

  private:
    uint32_t m_RendererID;
};

} // namespace zk
