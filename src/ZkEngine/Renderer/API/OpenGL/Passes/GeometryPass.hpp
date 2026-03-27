#pragma once

#include "ZkEngine/Renderer/Core/RenderPass.hpp"
#include "ZkEngine/Renderer/Core/RenderQueue.hpp"

namespace zk {

class OpenGLRenderer;

class GeometryPass : public RenderPass {
  public:
    GeometryPass(OpenGLRenderer* renderer);

    void execute(const RenderContext& ctx) override;
    void debug() override;

    void drawLine(const std::vector<glm::vec3>& pts);
    void clear();

  private:
    void draw(const RenderCommand& cmd);
    void uploadLightsToShader(Shader& shader, const LightData& data);
    void bindGlobalUniforms(Shader& shader);

  private:
    OpenGLRenderer* m_Renderer;

    Shader* m_CurrentShader = nullptr;

    bool m_EnableTextures = true;
    bool m_EnableBlinn = true;

    // Material shininess
    float m_Shininess = 64.0f;

    uint32_t m_LineVAO = 0, m_LineVBO = 0;
    struct LineStripCmd {
        std::vector<glm::vec3> points;
    };

    std::vector<LineStripCmd> m_LineQueue;
};

} // namespace zk
