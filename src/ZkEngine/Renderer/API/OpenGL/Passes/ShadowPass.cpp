#include "ShadowPass.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>

namespace zk {

ShadowPass::ShadowPass(OpenGLRenderer* renderer) : m_Renderer(renderer) {
    m_ShadowShader = Asset::loadShader("shaders/ShadowMap.vert", "shaders/ShadowMap.frag");
    m_ShadowCubeMapShader = Asset::loadShader(
        "shaders/ShadowCubeMap.vert", "shaders/ShadowCubeMap.frag", "shaders/ShadowCubeMap.geom");

    // Shadow FBO
    FramebufferSpec shadowSpecs;
    shadowSpecs.width = SHADOW_WIDTH;
    shadowSpecs.height = SHADOW_HEIGHT;
    shadowSpecs.attachments = {
        {TextureFormat::Depth32F, TextureType::Texture2D},
    };
    m_ShadowMapFBO = renderer->getDevice().createFramebuffer(shadowSpecs);

    // Depth Cube Map
    FramebufferSpec depthCubeMap;
    depthCubeMap.width = SHADOW_WIDTH;
    depthCubeMap.height = SHADOW_HEIGHT;
    depthCubeMap.attachments = {
        {TextureFormat::Depth32F, TextureType::TextureCubeMap},
    };
    m_ShadowCubeMapFBO = renderer->getDevice().createFramebuffer(depthCubeMap);
}

void ShadowPass::execute(const RenderContext& ctx) {
    for (auto& dirLight : ctx.lights->dirLights) {
        updateShadowMap(dirLight, *ctx.queue);
    }

    // for (auto& light : ctx.lights->pointLights) {
    //     updateShadowCubeMap(light, *ctx.queue);
    //
    //     // if (light.depthMap == 0)
    //     //     initPointLightsShadowMap(light);
    // }

    // for (auto& light : ctx.lights->spotLights) {
    //     updateShadowCubeMap(light, *ctx.queue);
}

void ShadowPass::updateShadowMap(GPUDirLight& dirLight, const RenderQueue& queue) {
    // glCullFace(GL_FRONT);

    float near_plane = 1.0f;
    float far_plane = 100.5f;

    // glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane,
    // far_plane);
    //
    // glm::mat4 lightView =
    //     glm::lookAt(m_LookAt1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float shadowDistance = 22.0f;
    glm::vec3 lightPos = -dirLight.light.direction * shadowDistance;

    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    // This fixes it?
    // glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, 30.0f, -30.0f, 1.0f, 60.0f);
    glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 60.0f);

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    m_ShadowShader->bind();
    m_ShadowShader->setMat4("u_LightSpaceMatrix", dirLight.lightSpaceMatrix);

    m_ShadowMapFBO->bind();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& cmd : queue.getOpaque()) {
        cmd.mesh->bind();

        m_ShadowShader->setMat4("u_Model", cmd.modelMatrix);

        m_Renderer->drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dirLight.depthMap, 0);

    m_ShadowMapFBO->unbind();

    glDisable(GL_DEPTH_TEST);
    // glCullFace(GL_BACK);

    dirLight.depthMap = m_ShadowMapFBO->getDepthAttachment();
    dirLight.lightSpaceMatrix = lightSpaceMatrix;
}

void ShadowPass::updateShadowCubeMap(GPUPointLight& light, const RenderQueue& queue) {
    float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float near = 1.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, light.farPlane);

    // Create 6 view matrices for each face of the cubemap
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(
            light.position, light.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    // Render to cubemap
    // Cubemap
    m_ShadowCubeMapFBO->bind();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_ShadowCubeMapShader->bind();

    for (size_t i = 0; i < shadowTransforms.size(); i++) {
        m_ShadowCubeMapShader->setMat4(
            "u_ShadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    }

    m_ShadowCubeMapShader->setVec3("u_LightPos", light.position);
    m_ShadowCubeMapShader->setFloat("u_FarPlane", light.farPlane);

    for (const auto& cmd : queue.getOpaque()) {
        cmd.mesh->bind();

        m_ShadowCubeMapShader->setMat4("u_Model", cmd.modelMatrix);

        m_Renderer->drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.depthMap, 0);

    m_ShadowCubeMapFBO->unbind();

    // glDisable(GL_DEPTH_TEST);
}

// void ShadowPass::initPointLightsShadowMap(GPUPointLight& light) {
//     glGenTextures(1, &light.depthMap);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthMap);
//
//     for (int i = 0; i < 6; i++) {
//         glTexImage2D(
//             GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//             0,
//             GL_DEPTH_COMPONENT,
//             SHADOW_WIDTH,
//             SHADOW_HEIGHT,
//             0,
//             GL_DEPTH_COMPONENT,
//             GL_FLOAT,
//             nullptr);
//     }
//
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// }

void ShadowPass::debug() {
    ImGui::Begin("Shadow Map");

    uint32_t shadowTex = m_ShadowMapFBO->getDepthAttachment();
    // uint32_t shadowTex = m_DepthCubeMapFBO->getDepthAttachment(); // Cant do this, its a cubemap

    ImGui::Image(
        (ImTextureID)(uintptr_t)shadowTex,
        ImVec2(512, 512),
        ImVec2(0, 1), // flip vertically (OpenGL)
        ImVec2(1, 0));

    ImGui::End();
}

} // namespace zk
