#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class VulkanRenderer : public Core::RendererBackend {
  public:
    void init() override;
    void update() override;
    void beginFrame() const override;
    void submit(const RenderObject& obj) override;
    void endFrame() override;
    void setClearColor(const glm::vec4& color) override;
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void debug() override;
};

} // namespace zk
