#pragma once

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Core/RenderPass.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include <memory>

namespace zk {

class PostProcessPass : public RenderPass {
  public:
    PostProcessPass() = default;
    ~PostProcessPass() = default;

    void execute(const RenderContext& ctx) override;
    void debug() override;

    void setInput(uint32_t texture) { m_InputTexture = texture; }
    void setOutput(Framebuffer* fbo) { m_OutputFBO = fbo; }

  private:
    void drawFullscreenQuad();

  private:
    uint32_t m_InputTexture = 0;
    Framebuffer* m_OutputFBO = nullptr;

    std::shared_ptr<Shader> m_Shader =
        Asset::loadShader("shaders/Screen.vert", "shaders/Screen.frag");

    uint32_t m_QuadVAO = 0;
    uint32_t m_QuadVBO = 0;

    bool m_GammaCorrection = true;
};

} // namespace zk
