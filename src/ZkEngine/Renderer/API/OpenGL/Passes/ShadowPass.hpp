#pragma once

#include "ZkEngine/Renderer/Core/RenderPass.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Scene/Lights.hpp"

#include <memory>

namespace zk {

class OpenGLRenderer;

class ShadowPass : public RenderPass {
  public:
    ShadowPass(OpenGLRenderer* renderer);
    void execute(const RenderContext& ctx) override;

    void debug() override;

  private:
    void updateShadowMap(GPUDirLight& dirLight, const RenderQueue& queue);
    void updateShadowCubeMap(GPUPointLight& light, const RenderQueue& queue);

  private:
    std::shared_ptr<Shader> m_ShadowShader;
    std::shared_ptr<Framebuffer> m_ShadowMapFBO;

    std::shared_ptr<Shader> m_ShadowCubeMapShader;
    std::shared_ptr<Framebuffer> m_ShadowCubeMapFBO;

    constexpr static int SHADOW_WIDTH = 1024;
    constexpr static int SHADOW_HEIGHT = 1024;

    OpenGLRenderer* m_Renderer;
};

} // namespace zk
