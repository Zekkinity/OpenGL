#include "zkpch.hpp"

#include "DeferredRenderer.hpp"

#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Utils/MeshFactory.hpp"

#include "imgui.h"
#include <random>

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
        {TextureFormat::Depth, TextureType::Texture2D}                          // Depth
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
    m_LightingShader =
        Asset::loadShader("shaders/GBufferLightingPass.vert", "shaders/GBufferLightingPass.frag");

    m_Shader = Asset::loadShader("shaders/Screen.vert", "shaders/Screen.frag");

    m_LightSource = Asset::loadShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    MeshData cube = MeshFactory::createCube();
    m_CubeMesh = device.createMesh(cube.vertices, cube.indices);

    // SSAO
    std::uniform_real_distribution<float> randomFloats(
        0.0, 1.0); // random floats between [0.0, 1.0]

    std::default_random_engine generator;

    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = (float)i / 64.0;
        scale = std::lerp(0.1f, 1.0f, scale * scale);

        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    std::vector<glm::vec3> ssaoNoise;
    for (int i = 0; i < 16; ++i) {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);
        ssaoNoise.push_back(noise);
    }

    m_NoiseTexture = m_Device->createTexture(
        {.width = 4,
         .height = 4,
         .format = TextureFormat::RGBA16F,
         .filter = TextureFilter::Nearest,
         .wrap = TextureWrap::Repeat},
        {.data = ssaoNoise.data()});

    FramebufferSpec ssaoSpecs;
    ssaoSpecs.width = 1920;
    ssaoSpecs.height = 1080;
    ssaoSpecs.attachments = {
        {TextureFormat::R8, TextureType::Texture2D, TextureFilter::Nearest},
    };
    m_SSAOFBO = m_Device->createFramebuffer(ssaoSpecs);

    m_SSAOShader = Asset::loadShader("shaders/SSAO.vert", "shaders/SSAO.frag");

    FramebufferSpec ssaoBlurSpec;
    ssaoBlurSpec.width = 1920;
    ssaoBlurSpec.height = 1080;
    ssaoBlurSpec.attachments = {
        {TextureFormat::R8, TextureType::Texture2D, TextureFilter::Nearest},
    };
    m_SSAOBlurFBO = m_Device->createFramebuffer(ssaoBlurSpec);

    m_SSAOBlurShader = Asset::loadShader("shaders/SSAOBlur.vert", "shaders/SSAOBlur.frag");
}

uint32_t DeferredRenderer::render(World& world, BatchRenderer& batchRenderer) {
    DrawContext ctx;
    ctx.shader = m_GBufferShader.get();
    ctx.bindShader = [](Shader& shader) { shader.bind(); };
    ctx.bindMaterial = [](Material* material, Shader& shader) { material->bindTextures(); };

    {
        ZK_PROFILE_SCOPE("Geometry Pass");
        geometryPass(ctx, batchRenderer);
    }
    if (m_EnableSSAO) {
        ZK_PROFILE_SCOPE("SSAO Pass");
        ssaoPass();
    }
    {
        ZK_PROFILE_SCOPE("Lighting Pass");
        lightingPass();
    }
    {
        ZK_PROFILE_SCOPE("Forward Shading Pass");
        // forwardShadingPass();
    }

    return m_LightingFBO->getColorAttachment(0);
    // return m_SSAOFBO->getColorAttachment(0);
}

void DeferredRenderer::geometryPass(const DrawContext& ctx, BatchRenderer& batchRenderer) {
    // Geometry Pass
    m_GBuffer->bind();
    glViewport(0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    batchRenderer.draw(ctx);

    m_GBuffer->unbind();
}

void DeferredRenderer::lightingPass() {
    m_LightingFBO->bind();

    // Lighting Pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        glBindTexture(GL_TEXTURE_2D, m_SSAOBlurFBO->getColorAttachment(0));
    else
        Asset::getWhiteTexture()->bind(4);

    // bind lighting shader
    m_LightingShader->bind();
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // bind gbuffer textures
    m_LightingShader->setInt("gPosition", 0);
    m_LightingShader->setInt("gNormal", 1);
    m_LightingShader->setInt("gAlbedoSpec", 2);
    m_LightingShader->setInt("gDepth", 3);
    m_LightingShader->setInt("ssao", 4);

    m_Device->drawScreenQuad();

    m_LightingShader->unbind();
    m_LightingFBO->unbind();
}

void DeferredRenderer::ssaoPass() {
    m_SSAOFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(0)); // position

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(1)); // normal

    m_NoiseTexture->bind(2);

    m_SSAOShader->bind();
    m_SSAOShader->setInt("gPosition", 0);
    m_SSAOShader->setInt("gNormal", 1);
    m_SSAOShader->setInt("texNoise", 2);

    for (int i = 0; i < 64; i++) {
        m_SSAOShader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }

    m_SSAOShader->setInt("u_KernelSize", m_SSAOKernelSize);
    m_SSAOShader->setFloat("u_Radius", m_SSAORadius);
    m_SSAOShader->setFloat("u_Bias", m_SSAOBias);

    m_Device->drawScreenQuad();

    m_SSAOShader->unbind();

    m_SSAOFBO->unbind();

    // BLUR PASS

    m_SSAOBlurFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SSAOFBO->getColorAttachment(0));

    m_SSAOBlurShader->bind();
    m_SSAOBlurShader->setInt("ssaoInput", 0);

    m_Device->drawScreenQuad();

    m_SSAOBlurShader->unbind();
    m_SSAOBlurFBO->unbind();
}

void DeferredRenderer::forwardShadingPass() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->getID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0,
        0,
        m_GBuffer->getWidth(),
        m_GBuffer->getHeight(),
        0,
        0,
        m_GBuffer->getWidth(),
        m_GBuffer->getHeight(),
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Forward Shading
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // or GL_LEQUAL

    m_LightSource->bind();

    // Draw Lights in Forward Shading
    // for (size_t i = 0; i < lights.size(); i++) {
    //     const auto& light = lights[i];
    //
    //     glm::mat4 model = glm::mat4(1.0f);
    //     model = glm::translate(model, light.position);
    //     model = glm::scale(model, glm::vec3(0.25f));
    //
    //     m_LightSource->setMat4("u_Model", model);
    //     m_LightSource->setVec4("u_Color", glm::vec4(light.color, 1.0));
    //
    //     m_Device->drawMesh(m_CubeMesh);
    // }
}

void DeferredRenderer::resize(uint32_t width, uint32_t height) {
    m_GBuffer->resize(width, height);
}

void DeferredRenderer::debug() {
    if (ImGui::Begin("GBuffer")) {
        ImGui::Checkbox("Enable SSAO", &m_EnableSSAO);
        ImGui::SliderInt("Kernel Size", &m_SSAOKernelSize, 1, 64);
        ImGui::SliderFloat("Radius", &m_SSAORadius, 0.0f, 1.0f);
        ImGui::SliderFloat("Bias", &m_SSAOBias, 0.0f, 0.1f);

        uint32_t ssaoTexture = m_SSAOFBO->getColorAttachment(0);
        ImGui::Image(
                (ImTextureID)(uintptr_t)ssaoTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
        uint32_t ssaoBlurTexture = m_SSAOBlurFBO->getColorAttachment(0);
        ImGui::Image(
                (ImTextureID)(uintptr_t)ssaoBlurTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        uint32_t posTexture = m_GBuffer->getColorAttachment(0);
        uint32_t normalTexture = m_GBuffer->getColorAttachment(1);
        uint32_t albedoTexture = m_GBuffer->getColorAttachment(2);
        uint32_t depthTexture = m_GBuffer->getDepthAttachment();

        ImGui::Image(
            (ImTextureID)(uintptr_t)posTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Image(
            (ImTextureID)(uintptr_t)normalTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Image(
            (ImTextureID)(uintptr_t)albedoTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Image(
            (ImTextureID)(uintptr_t)depthTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

    }
    ImGui::End();
}

} // namespace zk
