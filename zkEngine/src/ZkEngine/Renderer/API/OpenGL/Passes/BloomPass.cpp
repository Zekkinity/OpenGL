#include "zkpch.hpp"
#include "BloomPass.hpp"

#include "ZkEngine/Renderer/RenderDevice.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
#include "ZkEngine/Renderer/Core/RenderContext.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "imgui.h"

namespace zk {

BloomPass::BloomPass(RenderDevice* device) : m_Device(device) {
    glGenFramebuffers(1, &m_BloomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);

    glm::vec2 mipSize = glm::vec2(1920.0f, 1080.0f);
    glm::ivec2 mipIntSize = glm::ivec2(1920, 1080);

    const uint32_t BLOOM_MIP_COUNT = 5;

    for (uint32_t i = 0; i < BLOOM_MIP_COUNT; i++) {
        BloomMip mip;

        mipSize *= 0.5f;
        mipIntSize /= 2;
        mip.size = mipSize;
        mip.intSize = mipIntSize;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // we are downscaling an HDR color buffer, so we need a float texture format
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R11F_G11F_B10F,
            (int)mipSize.x,
            (int)mipSize.y,
            0,
            GL_RGB,
            GL_FLOAT,
            nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_MipChain.emplace_back(mip);
    }

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_MipChain[0].texture, 0);

    // setup attachments
    unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    // check completion status
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("gbuffer FBO error, status: 0x\%x\n", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_DownsampleShader = Asset::loadShader("shaders/Screen.vert", "shaders/Downsample.frag");
    m_UpsampleShader = Asset::loadShader("shaders/Screen.vert", "shaders/Upsample.frag");

    m_DownsampleShader->bind();
    m_DownsampleShader->setInt("srcTexture", 0);

    m_UpsampleShader->bind();
    m_UpsampleShader->setInt("srcTexture", 0);
}

BloomPass::~BloomPass() {
    for (int i = 0; i < m_MipChain.size(); i++) {
        glDeleteTextures(1, &m_MipChain[i].texture);
        m_MipChain[i].texture = 0;
    }

    glDeleteFramebuffers(1, &m_BloomFBO);
    m_BloomFBO = 0;
}

uint32_t BloomPass::execute(uint32_t srcTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);

    renderDownsamples(srcTexture);
    renderUpsamples();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);

    return m_MipChain[0].texture;
}

void BloomPass::renderDownsamples(uint32_t srcTexture) {
    m_DownsampleShader->bind();
    m_DownsampleShader->setVec2("srcResolution", glm::vec2(1920.0f, 1080.0f));

    // Bind srcTexture (HDR color buffer) as initial texture input
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    // Progressively downsample through the mip chain
    for (int i = 0; i < m_MipChain.size(); i++) {
        const BloomMip& mip = m_MipChain[i];
        glViewport(0, 0, mip.size.x, mip.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        m_Device->drawScreenQuad();

        // Set current mip resolution as srcResolution for next iteration
        m_DownsampleShader->setVec2("srcResolution", mip.size);
        // Set current mip as texture input for next iteration
        glBindTexture(GL_TEXTURE_2D, mip.texture);
    }

    m_DownsampleShader->unbind();
}

void BloomPass::renderUpsamples() {
    m_UpsampleShader->bind();
    m_UpsampleShader->setFloat("filterRadius", m_FilterRadius);

    // Enable additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = m_MipChain.size() - 1; i > 0; i--) {
        const BloomMip& mip = m_MipChain[i];
        const BloomMip& nextMip = m_MipChain[i - 1];

        // Bind viewport and texture from where to read
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mip.texture);

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

        // Render screen-filled quad of resolution of current mip
        m_Device->drawScreenQuad();
    }

    // Disable additive blending
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
    glDisable(GL_BLEND);

    m_UpsampleShader->unbind();
}

void BloomPass::debug() {
    ImGui::SliderFloat("Filter Radius", &m_FilterRadius, 0.0f, 0.01f);

    ImGui::Image(
        (ImTextureID)(uintptr_t)m_MipChain[0].texture,
        ImVec2(512, 512),
        ImVec2(0, 1),
        ImVec2(1, 0));
}

} // namespace zk
