#include "SkyboxPass.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

namespace zk {

SkyboxPass::SkyboxPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
    glGenVertexArrays(1, &m_EmptyVAO);
}

SkyboxPass::~SkyboxPass() {
    glDeleteVertexArrays(1, &m_EmptyVAO);
}

void SkyboxPass::execute(const RenderContext& ctx) {
    if (!m_CubemapTexture)
        return;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glm::mat4 view = ctx.camera->getView();
    glm::mat4 proj = ctx.camera->getProjection();
    view = glm::mat4(glm::mat3(view));

    m_SkyboxShader->bind();
    m_SkyboxShader->setMat4("u_InvProj", glm::inverse(proj));
    m_SkyboxShader->setMat4("u_InvView", glm::inverse(view));

    m_SkyboxShader->setInt("u_Skybox", 0);
    m_CubemapTexture->bind();

    glBindVertexArray(m_EmptyVAO);
    m_Renderer->drawArrays(GL_TRIANGLES, 3);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void SkyboxPass::setSkybox(const std::vector<std::filesystem::path>& tex) {
    m_CubemapTexture = m_Renderer->getDevice().loadTextureCube(tex);
}

} // namespace zk
