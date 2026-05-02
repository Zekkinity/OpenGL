// #pragma once
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class OpenGLRenderer;
// class Texture;
// class Shader;
//
// class SkyboxPass : public RenderPass {
//   public:
//     SkyboxPass(OpenGLRenderer* renderer);
//     ~SkyboxPass();
//
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//
//     void setSkybox(const Ref<Texture>& tex);
//
//   private:
//     Ref<Shader> m_SkyboxShader;
//     Ref<Texture> m_CubemapTexture;
//
//     uint32_t m_EmptyVAO = 0;
//
//     OpenGLRenderer* m_Renderer = nullptr;
// };
//
// } // namespace zk
