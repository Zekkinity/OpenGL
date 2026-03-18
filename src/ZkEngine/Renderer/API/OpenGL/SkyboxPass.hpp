#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class SkyboxPass : public RenderPass {
  public:
    SkyboxPass(Camera3D& camera) : m_Camera(camera) {}

    void execute() override {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        // glm::mat4 view = m_Camera.getView();
        // glm::mat4 proj = m_Camera.getProjection();
        // view = glm::mat4(glm::mat3(view));
        //
        // m_SkyboxShader->bind();
        // m_SkyboxShader->setMat4("u_InvProj", glm::inverse(proj));
        // m_SkyboxShader->setMat4("u_InvView", glm::inverse(view));
        //
        // m_SkyboxShader->setInt("u_Skybox", 0);
        // m_CubemapTexture->bind();
        //
        // glBindVertexArray(m_EmptyVAO);
        // drawArrays(GL_TRIANGLES, 3);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }

  private:
    Camera3D& m_Camera;
};

} // namespace zk
