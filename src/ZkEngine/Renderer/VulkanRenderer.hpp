#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class VulkanRenderer : public RendererBackend {
  public:
    void init() override;
    void update() override;
    void submit(const RenderCommand& cmd) override;

    void renderScene(Scene& scene) override;

    void setClearColor(const glm::vec4& color) override;
    void setViewport(const Viewport& viewport) override;
    void debug() override;
    void resize(uint32_t width, uint32_t height) override;
};

} // namespace zk
