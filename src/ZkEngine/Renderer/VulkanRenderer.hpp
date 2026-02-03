#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class VulkanRenderer : public Core::RendererBackend {
  public:
    void init() override;
    void update() override;
    void beginFrame() const override;
    void submit(const RenderObject& obj, const Camera3D& camera) override;
    void endFrame() override;
    void setClearColor(const glm::vec4& color) override;
    void debug() override;
};

} // namespace zk
