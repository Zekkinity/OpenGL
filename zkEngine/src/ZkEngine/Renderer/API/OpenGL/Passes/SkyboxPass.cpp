// #include "zkpch.hpp"
// #include "SkyboxPass.hpp"
//
// #include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
// #include "ZkEngine/Core/AppAcess.hpp"
// #include "ZkEngine/Renderer/Shader.hpp"
//
// namespace zk {
//
// SkyboxPass::SkyboxPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
//     m_SkyboxShader = Asset::loadShader("shaders/Sky.vert", "shaders/Sky.frag");
//
//     glGenVertexArrays(1, &m_EmptyVAO);
// }
//
// SkyboxPass::~SkyboxPass() {
//     glDeleteVertexArrays(1, &m_EmptyVAO);
// }
//
// void SkyboxPass::execute(const RenderContext& ctx, const RenderView& view) {
//     if (!m_CubemapTexture)
//         return;
//
//     glDepthFunc(GL_LEQUAL);
//     glDepthMask(GL_FALSE);
//
//     // view = glm::mat4(glm::mat3(view));
//
//     m_SkyboxShader->bind();
//     m_SkyboxShader->setInt("u_Skybox", 0);
//
//     m_CubemapTexture->bind();
//
//     glBindVertexArray(m_EmptyVAO);
//     m_Renderer->drawArrays(GL_TRIANGLES, 3);
//
//     glDepthFunc(GL_LESS);
//     glDepthMask(GL_TRUE);
// }
//
// void SkyboxPass::setSkybox(const Ref<Texture>& tex) {
//     m_CubemapTexture = tex;
// }
//
// } // namespace zk
