#include "GeometryPass.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

namespace zk {

GeometryPass::GeometryPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
    // Line stuff
    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

/*
    TODO:
    - Drawing order matters for blending and stencil
    - Blending
        - Draw all opaque objects first
        - Sort all transparent objects
        - Draw all transparent objects in sorted order
*/
void GeometryPass::execute(const RenderContext& ctx) {
    // glDisable(GL_BLEND);
    // glDepthMask(GL_TRUE);

    for (const auto& cmd : ctx.queue->getOpaque()) {
        Shader& shader = cmd.material->getShader();

        if (&shader != m_CurrentShader) {
            shader.bind();
            bindGlobalUniforms(shader);
            // TODO: Implement a UBO for lights insteaad of this
            uploadLightsToShader(shader, *ctx.lights);

            m_CurrentShader = &shader;
        }

        draw(cmd);
    }

    // TODO:
    // Order Independent Transparency

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);

    ctx.queue->sortTransparent();
    for (const auto& cmd : ctx.queue->getTransparent()) {
        draw(cmd);
    }

    // -------------------- //

    glBindVertexArray(m_LineVAO);
    for (const auto& cmd : m_LineQueue) {
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            cmd.points.size() * sizeof(glm::vec3),
            cmd.points.data(),
            GL_DYNAMIC_DRAW);

        // drawArrays(GL_LINE_STRIP, cmd.points.size());
        m_Renderer->drawArrays(GL_LINES, cmd.points.size());
    }
}

void GeometryPass::draw(const RenderCommand& cmd) {
    if (!cmd.mesh) {
        ZK_CORE_ERROR("No Mesh for render command");
        return;
    }

    if (cmd.mesh->getIndexBuffer().getCount() == 0) {
        ZK_CORE_ERROR("Mesh has zero indices");
        return;
    }

    if (!cmd.material) {
        ZK_CORE_ERROR("No Material for render command");
        return;
    }

    cmd.material->setTextures(m_EnableTextures);
    cmd.material->setShininess(m_Shininess);

    cmd.material->bind();
    cmd.mesh->bind();

    Shader& shader = cmd.material->getShader();

    shader.setMat4("u_Model", cmd.modelMatrix);
    shader.setInt("u_Blinn", m_EnableBlinn);
    shader.setFloat("time", glfwGetTime());

    if (cmd.mesh->isTriangle()) {
        m_Renderer->drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }
    else {
        m_Renderer->drawElements(GL_LINES, cmd.mesh->getIndexBuffer().getCount());
    }
}

void GeometryPass::uploadLightsToShader(Shader& shader, const LightData& data) {
    shader.setInt("u_PointLightCount", data.pointLights.size());

    for (size_t i = 0; i < data.pointLights.size(); i++) {
        auto& pointLight = data.pointLights[i];

        shader.setLight(
            "u_PointLights[" + std::to_string(i) + "]", pointLight.light, pointLight.position);

        // Bind shadow map
        int unit = 4 + i;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointLight.depthMap);

        shader.setInt("u_ShadowCubeMaps[" + std::to_string(i) + "]", unit);
        shader.setFloat("u_FarPlanes[" + std::to_string(i) + "]", pointLight.farPlane);
    }

    for (size_t i = 0; i < data.spotLights.size(); i++) {
        shader.setLight(
            // "u_SpotLights[" + std::to_string(i) + "]",
            "u_SpotLight",
            data.spotLights[i].light,
            data.spotLights[i].position);
    }

    for (size_t i = 0; i < data.dirLights.size(); i++) {
        auto& dirLight = data.dirLights[i];

        shader.setLight(
            // "u_DirLights[" + std::to_string(i) + "]", data.DirectionalLights[i]);
            "u_DirLights",
            dirLight.light);
    }

    // // Pass shadow maps
    // for (size_t i = 0; i < data.pointLights.size(); i++) {
    //     glActiveTexture(GL_TEXTURE4 + i);
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, data.pointLights[i].depthMap);
    //
    //     shader.setInt("u_ShadowMaps[" + std::to_string(i) + "]", 4 + i);
    // }

    shader.setInt("u_ShadowCubeMap", 4);
}

void GeometryPass::bindGlobalUniforms(Shader& shader) {
    // Bind scene texture
    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, m_ShadowMapFBO->getDepthAttachment());

    // glActiveTexture(GL_TEXTURE4);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubeMapFBO->getDepthAttachment());
    //
    // shader.setInt("u_ShadowCubeMap", 4);

    // shader.setVec3("u_LightPos", m_LightPos);
    // shader.setFloat("u_FarPlane", m_FarPlane);
    //
    // // Bind shadow map
    // shader.setInt("u_ShadowMap", 3);
    //
    // shader.setMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
    // shader.setVec3("u_LightDir", m_LightDir);
}

void GeometryPass::drawLine(const std::vector<glm::vec3>& pts) {
    m_LineQueue.emplace_back(LineStripCmd{pts});
}

void GeometryPass::clear() {
    m_LineQueue.clear();
}

void GeometryPass::debug() {
    ImGui::Checkbox("Enable Textures", &m_EnableTextures);
    ImGui::SliderFloat("Shininess", &m_Shininess, 0.0f, 360.0f);
    ImGui::Checkbox("Enable Blinn", &m_EnableBlinn);
}

} // namespace zk
