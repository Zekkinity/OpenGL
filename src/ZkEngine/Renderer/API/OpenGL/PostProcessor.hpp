#pragma once

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "imgui.h"

namespace zk {

class PostProcessor {
  public:
    PostProcessor() = default;
    ~PostProcessor() = default;

    // Could also just return a texture
    void process(uint32_t inputTexture, Framebuffer* fbo) {
        if (fbo) {
            fbo->bind();
        }
        else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // screen
        }

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->bind();

        // Bind scene texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);

        m_Shader->setInt("u_ScreenTexture", 0);
        m_Shader->setInt("u_GammaCorrection", m_GammaCorrection);

        drawFullscreenQuad();

        if (fbo) {
            fbo->unbind();
        }
    }

    void drawFullscreenQuad() {
        if (m_QuadVAO == 0) {
            float quadVertices[] = {                          // positions   // texCoords
                                    -1.0f, 1.0f,  0.0f, 1.0f, //
                                    -1.0f, -1.0f, 0.0f, 0.0f, //
                                    1.0f,  -1.0f, 1.0f, 0.0f, //

                                    -1.0f, 1.0f,  0.0f, 1.0f, //
                                    1.0f,  -1.0f, 1.0f, 0.0f, //
                                    1.0f,  1.0f,  1.0f, 1.0f};

            glGenVertexArrays(1, &m_QuadVAO);
            glGenBuffers(1, &m_QuadVBO);

            glBindVertexArray(m_QuadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        }

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void debug() {
        ImGui::Checkbox("Gamma Correction", &m_GammaCorrection);
    }

  private:
    std::shared_ptr<Shader> m_Shader =
        Asset::loadShader("shaders/Screen.vert", "shaders/Screen.frag");

    uint32_t m_QuadVAO = 0;
    uint32_t m_QuadVBO = 0;

    bool m_GammaCorrection = true;
};

} // namespace zk
