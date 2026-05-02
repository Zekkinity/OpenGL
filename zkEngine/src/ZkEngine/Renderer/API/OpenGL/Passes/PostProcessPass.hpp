// #pragma once
//
// #include "ZkEngine/Renderer/Core/RenderPass.hpp"
//
// namespace zk {
//
// class Shader;
// class Framebuffer;
//
// class PostProcessPass : public RenderPass {
//   public:
//     PostProcessPass();
//     ~PostProcessPass() = default;
//
//     void execute(const RenderContext& ctx, const RenderView& view) override;
//     void debug() override;
//
//     void setOutput(Framebuffer* fbo) { m_OutputFBO = fbo; }
//
//   private:
//     void drawFullscreenQuad();
//
//   private:
//     Framebuffer* m_OutputFBO = nullptr;
//
//     Ref<Shader> m_Shader;
//
//     uint32_t m_QuadVAO = 0;
//     uint32_t m_QuadVBO = 0;
//
//     bool m_GammaCorrection = true;
//     bool m_Kernel = true;
//     bool m_HDR = true;
//
//     float m_Exposure = 1.0f;
// };
//
// } // namespace zk
