#include "ZkEngine/Renderer/API/OpenGL/OpenGLFramebuffer.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLUtils.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"

namespace zk {

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpec& specs) : m_Specs(specs) {
    for (const auto& attachment : m_Specs.attachments) {
        if (OpenGLUtils::isDepthFormat(attachment.format)) {
            m_DepthAttachmentSpec = attachment;
        }
        else {
            m_ColorAttachmentsSpecs.push_back(attachment);
        }
    }

    invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() {
    clear();
}

void OpenGLFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void OpenGLFramebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::invalidate() {
    if (m_RendererID) {
        clear();
    }

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // ----------------
    // Color Attachment
    // ----------------
    if (!m_ColorAttachmentsSpecs.empty()) {
        m_ColorAttachments.resize(m_ColorAttachmentsSpecs.size());
        glGenTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

        for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);

            auto internalFormat =
                OpenGLUtils::textureInternalFormatToGL(m_ColorAttachmentsSpecs[i].format);
            auto format = OpenGLUtils::textureFormatToGL(m_ColorAttachmentsSpecs[i].format);

            // Create empty texture
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat,
                m_Specs.width,
                m_Specs.height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Attach texture to framebuffer
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
        }
    }

    // ----------------
    // Depth Attachment
    // ----------------
    if (m_DepthAttachmentSpec.format != TextureFormat::None) {
        if (m_DepthAttachmentSpec.type == TextureType::Texture2D) {
            glGenTextures(1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

            auto internalFormat =
                OpenGLUtils::textureInternalFormatToGL(m_DepthAttachmentSpec.format);
            auto format = OpenGLUtils::textureFormatToGL(m_DepthAttachmentSpec.format);
            auto type = OpenGLUtils::textureTypeToGL(m_DepthAttachmentSpec.format);

            // Create empty texture
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat,
                m_Specs.width,
                m_Specs.height,
                0,
                format,
                type,
                NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            float borderColor[] = {1.0, 1.0, 1.0, 1.0};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            // Attach texture to framebuffer
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        }
        else {
            glGenTextures(1, &m_DepthAttachment);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthAttachment);

            auto internalFormat =
                OpenGLUtils::textureInternalFormatToGL(m_DepthAttachmentSpec.format);
            auto format = OpenGLUtils::textureFormatToGL(m_DepthAttachmentSpec.format);
            auto type = OpenGLUtils::textureTypeToGL(m_DepthAttachmentSpec.format);

            // Create empty cubemap
            for (uint32_t i = 0; i < 6; i++) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    internalFormat,
                    m_Specs.width,
                    m_Specs.height,
                    0,
                    format,
                    type,
                    NULL);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // Attach texture to framebuffer
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment, 0);
        }

        // Tell OpenGL to not draw color to the buffer
        if (m_ColorAttachmentsSpecs.empty()) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ZK_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t OpenGLFramebuffer::getColorAttachment(uint32_t index) const {
    if (m_ColorAttachments.empty()) {
        ZK_CORE_ASSERT(false, "Framebuffer has no color attachment!");
    }

    return m_ColorAttachments[index];
}

uint32_t OpenGLFramebuffer::getDepthAttachment() const {
    if (!m_DepthAttachment) {
        ZK_CORE_ASSERT(false, "Framebuffer has no depth attachment!");
    }

    return m_DepthAttachment;
}

void OpenGLFramebuffer::resize(uint32_t width, uint32_t height) {
    m_Specs.width = width;
    m_Specs.height = height;

    invalidate();
}

void OpenGLFramebuffer::clear() {
    glDeleteFramebuffers(1, &m_RendererID);

    if (!m_ColorAttachments.empty()) {
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    }

    if (m_DepthAttachment) {
        glDeleteTextures(1, &m_DepthAttachment);
    }

    m_ColorAttachments.clear();
    m_DepthAttachment = 0;
}

} // namespace zk
