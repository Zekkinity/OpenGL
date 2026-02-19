#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/UniformBuffer.hpp"

namespace zk {

class OpenGLRenderer : public Core::RendererBackend {
  public:
    void init() override;

    void update() override;

    void beginFrame() const override;
    void submit(const RenderObject& obj) override;
    void submit(const RenderObject& obj, const Transform3D& transform) override;
    void endFrame() override;

    void drawLine(const std::vector<glm::vec3>& pts) override;
    void setClearColor(const glm::vec4& color) override;
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    void debug() override;

    void updateCamera(const Camera3D& camera) override;

  private:
    struct DrawCommand {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        glm::mat4 model;
        glm::vec4 color;
        bool colorOverride;
    };

    std::vector<DrawCommand> m_DrawQueue;

    bool m_Debug = true;

    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    bool m_WireFrame = false;

    int m_DrawCallCount = 0;

    bool m_DisableTextures = false;

    uint32_t m_LineVAO = 0, m_LineVBO = 0;
    struct LineStripCmd {
        std::vector<glm::vec3> points;
    };

    std::vector<LineStripCmd> m_LineQueue;

    std::unique_ptr<UniformBuffer> m_CameraUBO;
};

} // namespace zk
