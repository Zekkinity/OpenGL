#pragma once

#include "ZkEngine/Core/AppAcess.hpp"

#include "ZkEngine/Renderer/Core/RenderPass.hpp"
#include "ZkEngine/Renderer/Core/RenderContext.hpp"

namespace zk {

class OpenGLRenderer;

class SkyboxPass : public RenderPass {
  public:
    SkyboxPass(OpenGLRenderer* renderer);
    ~SkyboxPass();

    void execute(const RenderContext& ctx) override;

    void setSkybox(const std::vector<std::filesystem::path>& tex);

  private:
    std::shared_ptr<Shader> m_SkyboxShader =
        Asset::loadShader("shaders/Sky.vert", "shaders/Sky.frag");
    std::shared_ptr<Texture> m_CubemapTexture;

    uint32_t m_EmptyVAO = 0;

    OpenGLRenderer* m_Renderer = nullptr;
};

} // namespace zk
