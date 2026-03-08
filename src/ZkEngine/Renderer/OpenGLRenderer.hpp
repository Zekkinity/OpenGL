#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"

namespace zk {

enum class RenderMode {
    Forward,
    Deferred,
    Wireframe,
};

class OpenGLRenderer : public RendererBackend {
  public:
    void init() override;

    void update() override;

    void beginFrame() const override;
    void submit(const RenderCommand& cmd) override;
    void endFrame() override;

    void drawLine(const std::vector<glm::vec3>& pts) override;
    void setClearColor(const glm::vec4& color) override;
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    void debug() override;

    void updateCamera(const Camera3D& camera) override;

  private:
    void sortTransparentQueue();
    void draw(const RenderCommand& cmd);

  private:
    std::vector<RenderCommand> m_OpaqueQueue;
    std::vector<RenderCommand> m_TransparentQueue;

    bool m_Debug = true;

    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    RenderMode m_RenderMode = RenderMode::Forward;

    int m_DrawCallCount = 0;

    bool m_EnableTextures = true;

    uint32_t m_LineVAO = 0, m_LineVBO = 0;
    struct LineStripCmd {
        std::vector<glm::vec3> points;
    };

    std::vector<LineStripCmd> m_LineQueue;

    std::unique_ptr<UniformBuffer> m_CameraUBO;
};

} // namespace zk
