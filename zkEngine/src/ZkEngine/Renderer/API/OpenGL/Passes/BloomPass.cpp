// #include "zkpch.hpp"
// #include "BloomPass.hpp"
//
// #include "ZkEngine/Core/AppAcess.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
// #include "ZkEngine/Renderer/Core/RenderContext.hpp"
// #include "ZkEngine/Renderer/Framebuffer.hpp"
// #include "ZkEngine/Renderer/Shader.hpp"
// #include "imgui.h"
//
// namespace zk {
//
// BloomPass::BloomPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
//     FramebufferSpec blurSpec;
//     blurSpec.width = 1920;
//     blurSpec.height = 1080;
//     blurSpec.attachments = {
//         {TextureFormat::RGBA16F, TextureType::Texture2D},
//     };
//
//     m_BlurFBO[0] = m_Renderer->getDevice().createFramebuffer(blurSpec);
//     m_BlurFBO[1] = m_Renderer->getDevice().createFramebuffer(blurSpec);
//
//     m_BlurShader = Asset::loadShader("shaders/Screen.vert", "shaders/Blur.frag");
// }
//
// BloomPass::~BloomPass() {}
//
// void BloomPass::execute(const RenderContext& ctx, const RenderView& view) {
//     if (!m_Enabled) {
//         m_OutputTexture = 0;
//         return;
//     }
//
//     glDisable(GL_DEPTH_TEST);
//
//     uint32_t brigthtTexture = ctx.brightColor;
//     bool horizontal = true;
//     bool firstIteration = true;
//
//     m_BlurShader->bind();
//     m_BlurShader->setInt("image", 0);
//
//     glViewport(0, 0, m_BlurFBO[0]->getWidth(), m_BlurFBO[0]->getHeight());
//     for (int i = 0; i < m_Iterations; i++) {
//         m_BlurFBO[horizontal]->bind();
//
//         glClear(GL_COLOR_BUFFER_BIT);
//
//         m_BlurShader->setInt("horizontal", horizontal);
//
//         glActiveTexture(GL_TEXTURE0);
//
//         if (firstIteration) {
//             glBindTexture(GL_TEXTURE_2D, brigthtTexture);
//         }
//         else {
//             glBindTexture(GL_TEXTURE_2D, m_BlurFBO[!horizontal]->getColorAttachment(0));
//         }
//
//         drawFullscreenQuad();
//
//         horizontal = !horizontal;
//
//         if (firstIteration)
//             firstIteration = false;
//     }
//
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//     m_OutputTexture = m_BlurFBO[!horizontal]->getColorAttachment(0);
// }
//
// void BloomPass::debug() {
//     ImGui::Checkbox("Enabled", &m_Enabled);
//     ImGui::SliderInt("Iterations", &m_Iterations, 1, 10);
//     ImGui::Text("Output Texture: %d", m_OutputTexture);
//
//     ImGui::Image(
//         (ImTextureID)(uintptr_t)m_OutputTexture,
//         ImVec2(512, 512),
//         ImVec2(0, 1), // flip vertically (OpenGL)
//         ImVec2(1, 0));
// }
//
// void BloomPass::drawFullscreenQuad() {
//     if (m_QuadVAO == 0) {
//         float quadVertices[] = {                          // positions   // texCoords
//                                 -1.0f, 1.0f,  0.0f, 1.0f, //
//                                 -1.0f, -1.0f, 0.0f, 0.0f, //
//                                 1.0f,  -1.0f, 1.0f, 0.0f, //
//
//                                 -1.0f, 1.0f,  0.0f, 1.0f, //
//                                 1.0f,  -1.0f, 1.0f, 0.0f, //
//                                 1.0f,  1.0f,  1.0f, 1.0f};
//
//         glGenVertexArrays(1, &m_QuadVAO);
//         glGenBuffers(1, &m_QuadVBO);
//
//         glBindVertexArray(m_QuadVAO);
//         glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//
//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(
//             1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//     }
//
//     glBindVertexArray(m_QuadVAO);
//     glDrawArrays(GL_TRIANGLES, 0, 6);
// }
//
// } // namespace zk
