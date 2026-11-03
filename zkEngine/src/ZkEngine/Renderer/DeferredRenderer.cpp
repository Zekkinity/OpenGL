#include "zkpch.hpp"

#include "DeferredRenderer.hpp"

#include "ZkEngine/Renderer/Core/FrameData.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Utils/MeshFactory.hpp"

#include "imgui.h"

namespace zk {

// NOTE:
// Remove Position and instead only store depth, then rebuild in lighting pass
// Can make a Z pre pass to remove overdraw
// Lighting and stuff before SSAO should be in world space
// SSAO and post processing stuff should move everything into view space

void DeferredRenderer::init(RenderDevice& device) {
    m_Device = &device;

    // DEFERRED
    FramebufferSpec gBufferSpecs;
    gBufferSpecs.width = 1920;
    gBufferSpecs.height = 1080;
    gBufferSpecs.attachments = {
        // NOTE:
        // For position and albedo 16bit or 32bit float is recommended
        // They should be NEAREST but by default they are LINEAR
        {TextureFormat::RGBA16F, TextureType::Texture2D, TextureFilter::Nearest}, // Position
        {TextureFormat::RGBA16F, TextureType::Texture2D, TextureFilter::Nearest}, // Normal
        {TextureFormat::RGBA8, TextureType::Texture2D, TextureFilter::Nearest}, // Albedo + Specular
        {TextureFormat::RGBA8,
         TextureType::Texture2D,
         TextureFilter::Nearest},                       // Metallic + Roughness
        {TextureFormat::Depth, TextureType::Texture2D}, // Depth
    };

    m_GBuffer = device.createFramebuffer(gBufferSpecs);
    m_GBufferShader = Asset::loadShader("shaders/GBuffer.vert", "shaders/GBuffer.frag");

    FramebufferSpec lightingSpecs;
    lightingSpecs.width = 1920;
    lightingSpecs.height = 1080;
    lightingSpecs.attachments = {
        {TextureFormat::RGBA16F, TextureType::Texture2D, TextureFilter::Nearest},
    };
    m_LightingFBO = device.createFramebuffer(lightingSpecs);
    // m_LightingShader =
    //     Asset::loadShader("shaders/GBufferLightingPass.vert",
    //     "shaders/GBufferLightingPass.frag");
    m_LightingShader = Asset::loadShader("shaders/PBR.vert", "shaders/PBR.frag");

    m_LightSource = Asset::loadShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    MeshData cube = MeshFactory::createSphere();
    m_CubeMesh = device.createMesh(cube.vertices, cube.indices);

    m_SSAOPass = createScope<SSAOPass>(m_Device);
}

uint32_t DeferredRenderer::render(FrameData& frameData, BatchRenderer& batchRenderer) {
    {
        ZK_PROFILE_SCOPE("Geometry Pass");
        geometryPass(batchRenderer);
    }

    uint32_t ssaoTexture = 0;
    if (m_EnableSSAO) {
        ZK_PROFILE_SCOPE("SSAO Pass");
        ssaoTexture =
            m_SSAOPass->execute(m_GBuffer->getColorAttachment(0), m_GBuffer->getColorAttachment(1));
    }
    {
        ZK_PROFILE_SCOPE("Lighting Pass");
        lightingPass(frameData, ssaoTexture);
    }

    return m_LightingFBO->getColorAttachment(0);
    // return m_SSAOFBO->getColorAttachment(0);
}

void DeferredRenderer::geometryPass(BatchRenderer& batchRenderer) {
    // Geometry Pass
    m_GBuffer->bind();
    glViewport(0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    Shader* currentShader = nullptr;
    Material* currentMaterial = nullptr;

    for (const auto& batch : batchRenderer.getBatches()) {
        Material* material = batch.key.material.get();
        Shader* shader = &material->getShader();

        // Update shader
        if (shader != currentShader) {
            shader->bind();
            currentShader = shader;
        }

        // Update material
        if (material != currentMaterial) {
            material->setNormalMapping(m_EnableNormalMapping);
            material->bindTextures();
            material->uploadUniforms(*shader);

            currentMaterial = material;
        }

        batch.key.mesh->bind();

        m_Device->drawBatch(batch);
    }

    m_GBuffer->unbind();
}

void DeferredRenderer::lightingPass(FrameData& frameData, uint32_t ssaoTexture) {
    m_LightingFBO->bind();

    // Lighting Pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_LightingShader->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(0));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(1));

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(2));

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getDepthAttachment());

    glActiveTexture(GL_TEXTURE4);
    if (m_EnableSSAO)
        glBindTexture(GL_TEXTURE_2D, ssaoTexture);
    else
        Asset::getWhiteTexture()->bind(4);

    // Metallic + Roughness
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(3));

    // TODO: Provide shadow map for directional light and the u_LightMatrix (lightspacematrix)

    // FIX: This will crash if theres no directional light
    auto* dirLight = &frameData.lights.dirLights[0];
    if (dirLight) {

        // Shadow Map
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, dirLight->depthMap);

        // Pass Shadow Data
        m_LightingShader->setMat4("u_DirLightMatrix", dirLight->lightSpaceMatrix);
    } else {
        glActiveTexture(GL_TEXTURE6);
        Asset::getWhiteTexture()->bind(6);
    }

    m_LightingShader->setInt("u_DirShadowMap", 6);

    // bind lighting shader
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // bind gbuffer textures
    m_LightingShader->setInt("gPosition", 0);
    m_LightingShader->setInt("gNormal", 1);
    m_LightingShader->setInt("gAlbedoSpec", 2);
    m_LightingShader->setInt("gDepth", 3);
    m_LightingShader->setInt("ssao", 4);
    m_LightingShader->setInt("gMetallicRoughness", 5);

    // m_LightingShader->setFloat("metallic", 0.5);
    // m_LightingShader->setFloat("roughness", 0.5);
    m_LightingShader->setFloat("ao", 1.0);


    m_Device->drawScreenQuad();

    m_LightingShader->unbind();
    m_LightingFBO->unbind();
}

void DeferredRenderer::resize(uint32_t width, uint32_t height) {
    m_GBuffer->resize(width, height);
    m_LightingFBO->resize(width, height);
    m_SSAOPass->resize(width, height);
}

void DeferredRenderer::debug() {
    if (ImGui::Begin("GBuffer")) {
        ImGui::Checkbox("Enable Normal Mapping", &m_EnableNormalMapping);
        ImGui::Checkbox("Enable SSAO", &m_EnableSSAO);

        if (m_EnableSSAO)
            m_SSAOPass->debug();

        uint32_t posTexture = m_GBuffer->getColorAttachment(0);
        uint32_t normalTexture = m_GBuffer->getColorAttachment(1);
        uint32_t albedoTexture = m_GBuffer->getColorAttachment(2);
        uint32_t depthTexture = m_GBuffer->getDepthAttachment();
        uint32_t metallicRoughnessTexture = m_GBuffer->getColorAttachment(3);

        ImGui::Text("Position");
        ImGui::Image(
            (ImTextureID)(uintptr_t)posTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Normal");
        ImGui::Image(
            (ImTextureID)(uintptr_t)normalTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Albedo & Specular");
        ImGui::Image(
            (ImTextureID)(uintptr_t)albedoTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Depth");
        ImGui::Image(
            (ImTextureID)(uintptr_t)depthTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Metallic & Roughness");
        ImGui::Image(
            (ImTextureID)(uintptr_t)metallicRoughnessTexture,
            ImVec2(512, 512),
            ImVec2(0, 1),
            ImVec2(1, 0));
    }
    ImGui::End();
}

} // namespace zk
