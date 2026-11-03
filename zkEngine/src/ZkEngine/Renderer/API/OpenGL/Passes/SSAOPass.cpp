#include "zkpch.hpp"

#include "SSAOPass.hpp"

#include "ZkEngine/Core/AppAcess.hpp"

#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Shader.hpp"

#include "imgui.h"
#include <random>

namespace zk {

SSAOPass::SSAOPass(RenderDevice* device) : m_Device(device) {
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

SSAOPass::~SSAOPass() {}

uint32_t SSAOPass::execute(uint32_t position, uint32_t normal) {
    m_SSAOFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_GBuffer->getColorAttachment(0)); // position
    glBindTexture(GL_TEXTURE_2D, position); // position

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal); // normal

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

    return m_SSAOBlurFBO->getColorAttachment(0);
}

void SSAOPass::resize(uint32_t width, uint32_t height) {
    m_SSAOFBO->resize(width, height);
    m_SSAOBlurFBO->resize(width, height);
}

void SSAOPass::debug() {
    ImGui::SliderInt("Kernel Size", &m_SSAOKernelSize, 1, 64);
    ImGui::SliderFloat("Radius", &m_SSAORadius, 0.0f, 1.0f);
    ImGui::SliderFloat("Bias", &m_SSAOBias, 0.0f, 0.1f);

    uint32_t ssaoTexture = m_SSAOFBO->getColorAttachment(0);
    ImGui::Image((ImTextureID)(uintptr_t)ssaoTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
    uint32_t ssaoBlurTexture = m_SSAOBlurFBO->getColorAttachment(0);
    ImGui::Image(
        (ImTextureID)(uintptr_t)ssaoBlurTexture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
}

} // namespace zk
