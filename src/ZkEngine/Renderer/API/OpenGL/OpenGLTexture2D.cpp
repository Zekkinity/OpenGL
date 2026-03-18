#include "ZkEngine/Renderer/API/OpenGL/OpenGLTexture2D.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLUtils.hpp"

#include "ZkEngine/Core/Base.hpp"

namespace zk {

/*
    You should be careful when specifying your textures in sRGB space as not all textures will
    actually be in sRGB space. Textures used for coloring objects (like diffuse textures) are almost
    always in sRGB space. Textures used for retrieving lighting parameters (like specular maps and
    normal maps) are almost always in linear space, so if you were to configure these as sRGB
    textures the lighting will look odd. Be careful in which textures you specify as sRGB.

    With our diffuse textures specified as sRGB textures you get the visual output you'd expect
    again, but this time everything is gamma corrected only once.
*/

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecs& specs, ImageData image) : m_Specs(specs) {
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    auto internalFormat = OpenGLUtils::textureInternalFormatToGL(specs.format);
    auto format = OpenGLUtils::textureFormatToGL(specs.format);

    // GL_RGB12
    // GL_SRGB8_ALPHA8,
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        specs.width,
        specs.height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        image.data);

    auto filter = OpenGLUtils::textureFilterToGL(specs.filter);
    auto wrap = OpenGLUtils::textureWrapToGL(specs.wrap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void OpenGLTexture2D::unbind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace zk
