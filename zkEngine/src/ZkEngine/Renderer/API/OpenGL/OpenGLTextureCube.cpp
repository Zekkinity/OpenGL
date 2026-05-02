#include "zkpch.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLTextureCube.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLUtils.hpp"

#include <glad/glad.h>

namespace zk {

OpenGLTextureCube::OpenGLTextureCube(
    const TextureSpecs& specs, const std::array<ImageData, 6>& images)
    : m_Specs(specs) {
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

    auto internalFormat = OpenGLUtils::textureInternalFormatToGL(specs.format);
    auto format = OpenGLUtils::textureFormatToGL(specs.format);

    for (uint32_t i = 0; i < 6; i++) {
        const void* data = images[i].data;
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            specs.width,
            specs.height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data);
    }

    auto filter = OpenGLUtils::textureFilterToGL(specs.filter);
    auto wrap = OpenGLUtils::textureWrapToGL(specs.wrap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

OpenGLTextureCube::~OpenGLTextureCube() {
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTextureCube::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}

void OpenGLTextureCube::unbind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

} // namespace zk
