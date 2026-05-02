// #pragma once
//
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class OpenGLRenderer;
// class RenderQueue;
// struct GPUDirLight;
// struct GPUPointLight;
// class Shader;
// class Framebuffer;
//
// class ShadowPass : public RenderPass {
//   public:
//     ShadowPass(OpenGLRenderer* renderer);
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//
//     void debug() override;
//
//   private:
//     void updateShadowMap(GPUDirLight& dirLight, const RenderContext& ctx);
//     void updateShadowCubeMap(GPUPointLight& light, const RenderQueue& queue, Ref<Framebuffer>& fbo);
//
//   private:
//     Ref<Shader> m_ShadowShader;
//     Ref<Framebuffer> m_ShadowMapFBO;
//
//     Ref<Shader> m_ShadowCubeMapShader;
//     Ref<Framebuffer> m_ShadowCubeMapFBO;
//
//     std::vector<Ref<Framebuffer>> m_ShadowCubeMapFBOs;
//
//     constexpr static int SHADOW_WIDTH = 1024;
//     constexpr static int SHADOW_HEIGHT = 1024;
//
//     float m_ShadowDistance = 40.0f;
//     float m_Size = 60.0f;
//     float m_NearPlane = 1.0f;
//     float m_FarPlane = 100.0f;
//
//     OpenGLRenderer* m_Renderer;
// };
//
// } // namespace zk
