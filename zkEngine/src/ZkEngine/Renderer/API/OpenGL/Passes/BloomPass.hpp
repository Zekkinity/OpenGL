// #pragma once
//
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class Shader;
// class Framebuffer;
// class OpenGLRenderer;
//
// class BloomPass : public RenderPass {
//   public:
//     BloomPass(OpenGLRenderer* renderer);
//     ~BloomPass();
//
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//     void debug() override;
//
//     uint32_t getOutputTexture() const { return m_OutputTexture; }
//
//   private:
//     void drawFullscreenQuad();
//
//   private:
//     OpenGLRenderer* m_Renderer = nullptr;
//
//     uint32_t m_OutputTexture = 0;
//
//     std::array<Scope<Framebuffer>, 2> m_BlurFBO;
//     Ref<Shader> m_BlurShader;
//
//     int m_Iterations = 3;
//
//     uint32_t m_QuadVAO = 0;
//     uint32_t m_QuadVBO = 0;
// };
//
// } // namespace zk
