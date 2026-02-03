#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class OpenGLRenderer : public Core::RendererBackend {
  public:
    void init() override;

    void update() override;

    void beginFrame() const override;
    void submit(const RenderObject& obj, const Camera3D& camera) override;
    void endFrame() override;

    void setClearColor(const glm::vec4& color) override;

    void debug() override;

  private:
    struct DrawCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        glm::mat4 model;
        glm::mat4 viewProj;
        glm::vec4 color;
        bool colorOverride;
    };

    std::vector<DrawCommand> m_DrawQueue;

    bool m_Debug = true;

    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    bool m_WireFrame = false;

    int m_DrawCallCount = 0;
};

} // namespace zk
