#include "zkpch.hpp"
#include "ShadowPass.hpp"

#include "ZkEngine/World/Entity.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Core/RenderContext.hpp"
#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Core/FrameData.hpp"

#include "glm/ext/matrix_clip_space.hpp"

#include "imgui.h"

namespace zk {

ShadowPass::ShadowPass(RenderDevice* device) : m_Device(device) {
    m_ShadowShader = Asset::loadShader("shaders/ShadowMap.vert", "shaders/ShadowMap.frag");
    m_ShadowCubeMapShader = Asset::loadShader(
        "shaders/ShadowCubeMap.vert", "shaders/ShadowCubeMap.frag", "shaders/ShadowCubeMap.geom");

    uint32_t shadowSize = getShadowSize(m_Quality);

    // Shadow FBO
    FramebufferSpec shadowSpecs;
    shadowSpecs.width = shadowSize;
    shadowSpecs.height = shadowSize;
    shadowSpecs.attachments = {
        {TextureFormat::Depth32F, TextureType::Texture2D},
    };
    m_ShadowMapFBO = device->createFramebuffer(shadowSpecs);

    // Depth Cube Map
    FramebufferSpec depthCubeMap;
    depthCubeMap.width = shadowSize;
    depthCubeMap.height = shadowSize;
    depthCubeMap.attachments = {
        {TextureFormat::Depth32F, TextureType::TextureCubeMap},
    };
    m_ShadowCubeMapFBO = device->createFramebuffer(depthCubeMap);
}

void ShadowPass::execute(FrameData& frameData, const RenderQueue& queue) {
    // if (!m_Enabled) {
    //     // TODO: Return a valid default texture
    //     return;
    // }

    auto& lights = frameData.lights;
    auto& camera = frameData.camera;

    // Spot Lights
    for (auto& spotLight : lights.spotLights) {
        updateSpotlight(spotLight, queue);
    }

    // Directional Lights
    for (auto& dirLight : lights.dirLights) {
        updateShadowMap(dirLight, camera, queue);
    }

    // Resize Shadow Cube Map FBOs
    while (m_ShadowCubeMapFBOs.size() < lights.pointLights.size()) {
        uint32_t shadowSize = getShadowSize(m_Quality);

        FramebufferSpec spec;
        spec.width = shadowSize;
        spec.height = shadowSize;
        spec.attachments = {
            {TextureFormat::Depth32F, TextureType::TextureCubeMap},
        };
        m_ShadowCubeMapFBOs.push_back(m_Device->createFramebuffer(spec));
    }

    static int current = 0;
    for (size_t i = 0; i < lights.pointLights.size(); i++) {
        // auto& pointLight = lights.pointLights[i];
        updateShadowCubeMap(lights.pointLights[current], queue, m_ShadowCubeMapFBOs[i]);
        current = (current + 1) % lights.pointLights.size();
    }
}

// This writes to dirLight.shadowMap and dirLight.lightSpaceMatrix
void ShadowPass::updateShadowMap(
    GPUDirLight& dirLight, const CameraData& camera, const RenderQueue& queue) {
    // glCullFace(GL_FRONT);

    // glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane,
    // far_plane);
    //
    // glm::mat4 lightView =
    //     glm::lookAt(m_LookAt1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 center = camera.position;
    glm::vec3 lightPos = center - dirLight.light.direction * m_ShadowDistance;

    glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0, 1, 0));

    // This fixes it?
    // glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, 30.0f, -30.0f, 1.0f, 60.0f);
    glm::mat4 lightProjection =
        glm::ortho(-m_Size, m_Size, -m_Size, m_Size, m_NearPlane, m_FarPlane);

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    m_ShadowShader->bind();
    m_ShadowShader->setMat4("u_LightSpaceMatrix", lightSpaceMatrix);

    uint32_t shadowSize = getShadowSize(m_Quality);
    m_ShadowMapFBO->bind();
    glViewport(0, 0, shadowSize, shadowSize);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // NOTE: Shadow pass is not doing instancing
    // NOTE: Should only draw what is in the view frustum of the light
    for (const auto& cmd : queue.getOpaque()) {
        m_ShadowShader->setMat4("u_Model", cmd.modelMatrix);

        if (cmd.material->getDiffuse()) {
            cmd.material->getDiffuse()->bind(0);
            m_ShadowShader->setInt("u_DiffuseTexture", 0);
            glDisable(GL_CULL_FACE);
        }

        m_Device->drawMesh(cmd.mesh);
    }
    glEnable(GL_CULL_FACE);

    m_ShadowMapFBO->unbind();

    glDisable(GL_DEPTH_TEST);
    // glCullFace(GL_BACK);

    dirLight.depthMap = m_ShadowMapFBO->getDepthAttachment();
    dirLight.lightSpaceMatrix = lightSpaceMatrix;
}

void ShadowPass::updateSpotlight(GPUSpotLight& spotLight, const RenderQueue& queue) {
    // glCullFace(GL_FRONT);

    glm::mat4 lightView = glm::lookAt(
        spotLight.position, spotLight.position + spotLight.light.direction, glm::vec3(0, 1, 0));

    glm::mat4 lightProjection =
        glm::perspective(glm::radians(spotLight.light.outerCutOff * 2.0f), 1.0f, 1.0f, 60.0f);

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    m_ShadowShader->bind();
    m_ShadowShader->setMat4("u_LightSpaceMatrix", lightSpaceMatrix);

    uint32_t shadowSize = getShadowSize(m_Quality);
    m_ShadowMapFBO->bind();
    glViewport(0, 0, shadowSize, shadowSize);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // NOTE: Shadow pass is not doing instancing
    // NOTE: Should only draw what is in the view frustum of the light
    for (const auto& cmd : queue.getOpaque()) {
        m_ShadowShader->setMat4("u_Model", cmd.modelMatrix);

        if (cmd.material->getDiffuse()) {
            cmd.material->getDiffuse()->bind(0);
            m_ShadowShader->setInt("u_DiffuseTexture", 0);
            glDisable(GL_CULL_FACE);
        }

        m_Device->drawMesh(cmd.mesh);
    }
    glEnable(GL_CULL_FACE);

    m_ShadowMapFBO->unbind();

    glDisable(GL_DEPTH_TEST);
    // glCullFace(GL_BACK);

    spotLight.depthMap = m_ShadowMapFBO->getDepthAttachment();
    spotLight.lightSpaceMatrix = lightSpaceMatrix;
}

void ShadowPass::updateShadowCubeMap(
    GPUPointLight& pointLight, const RenderQueue& queue, Ref<Framebuffer>& fbo) {
    uint32_t shadowSize = getShadowSize(m_Quality);

    float aspect = (float)shadowSize / (float)shadowSize;
    float near = 1.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, pointLight.farPlane);

    // Create 6 view matrices for each face of the cubemap
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(1.0, 0.0, 0.0),
                         glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(-1.0, 0.0, 0.0),
                         glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(0.0, 1.0, 0.0),
                         glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(0.0, -1.0, 0.0),
                         glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(0.0, 0.0, 1.0),
                         glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj * glm::lookAt(
                         pointLight.position,
                         pointLight.position + glm::vec3(0.0, 0.0, -1.0),
                         glm::vec3(0.0, -1.0, 0.0)));

    // Render to cubemap
    // Cubemap
    fbo->bind();
    glViewport(0, 0, shadowSize, shadowSize);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_ShadowCubeMapShader->bind();

    for (size_t i = 0; i < shadowTransforms.size(); i++) {
        m_ShadowCubeMapShader->setMat4(
            "u_ShadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    }

    m_ShadowCubeMapShader->setVec3("u_LightPos", pointLight.position);
    m_ShadowCubeMapShader->setFloat("u_FarPlane", pointLight.farPlane);

    float maxDistance = pointLight.farPlane;
    for (const auto& cmd : queue.getOpaque()) {
        // NOTE: DO Light culling
        float distance = glm::length(cmd.modelMatrix[3]);
        if (distance > maxDistance) {
            continue;
        }

        m_ShadowCubeMapShader->setMat4("u_Model", cmd.modelMatrix);

        m_Device->drawMesh(cmd.mesh);
    }

    fbo->unbind();

    pointLight.depthMap = fbo->getDepthAttachment();

    // glDisable(GL_DEPTH_TEST);
}

void ShadowPass::debug() {
    // ImGui::Checkbox("Enabled", &m_Enabled);
    ImGui::SliderInt("Quality", (int*)&m_Quality, 0, 2);
    const char* qualityNames[] = {
        "Low (1024x1024)",
        "Medium (2048x2048)",
        "High (4096x4096)",
        "Ultra (8192x8192)",
    };

    int current = static_cast<int>(m_Quality);

    if (ImGui::Combo("Shadow Quality", &current, qualityNames, IM_ARRAYSIZE(qualityNames))) {
        setQuality(static_cast<ShadowQuality>(current));
    }

    ImGui::SliderFloat("Shadow Distance", &m_ShadowDistance, 1.0f, 100.0f);
    ImGui::SliderFloat("Shadow Size", &m_Size, 0.0f, 100.0f);
    ImGui::SliderFloat("Near Plane", &m_NearPlane, 1.0f, 100.0f);
    ImGui::SliderFloat("Far Plane", &m_FarPlane, 1.0f, 100.0f);

    uint32_t shadowTex = m_ShadowMapFBO->getDepthAttachment();
    // uint32_t shadowTex = m_DepthCubeMapFBO->getDepthAttachment(); // Cant do this, its a
    // cubemap

    ImGui::Image(
        (ImTextureID)(uintptr_t)shadowTex,
        ImVec2(512, 512),
        ImVec2(0, 1), // flip vertically (OpenGL)
        ImVec2(1, 0));
}

uint32_t ShadowPass::getShadowSize(ShadowQuality quality) {
    switch (quality) {
    case ShadowQuality::LOW:
        return 1024;

    case ShadowQuality::MEDIUM:
        return 2048;

    case ShadowQuality::HIGH:
        return 4096;

    case ShadowQuality::ULTRA:
        return 8192;
    }
}

void ShadowPass::setQuality(ShadowQuality quality) {
    if (m_Quality == quality)
        return;

    m_Quality = quality;

    uint32_t shadowSize = getShadowSize(quality);

    m_ShadowMapFBO->resize(shadowSize, shadowSize);
    m_ShadowCubeMapFBO->resize(shadowSize, shadowSize);
}

} // namespace zk
