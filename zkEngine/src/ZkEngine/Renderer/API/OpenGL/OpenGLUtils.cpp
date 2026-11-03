#include "zkpch.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLUtils.hpp"

#include <glad/glad.h>

namespace zk::OpenGLUtils {

GLTextureFormat textureFormatToGL(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:
        return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};

    case TextureFormat::RGB8:
        return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};

    case TextureFormat::RGBA8:
        return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};

    case TextureFormat::SRGB:
        return {GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE};

    case TextureFormat::RGBA16F:
        return {GL_RGBA16F, GL_RGBA, GL_FLOAT};

    case TextureFormat::Depth:
        return {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};

    case TextureFormat::Depth32F:
        return {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT};

    case TextureFormat::Depth24Stencil8:
        return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
    }

    return {0, 0, 0};
}

uint32_t textureFilterToGL(TextureFilter filter) {
    switch (filter) {
    case TextureFilter::Nearest:
        return GL_NEAREST;
    case TextureFilter::Linear:
        return GL_LINEAR;
    }

    return GL_LINEAR;
}

uint32_t textureWrapToGL(TextureWrap wrap) {
    switch (wrap) {
    case TextureWrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case TextureWrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case TextureWrap::Repeat:
        return GL_REPEAT;
    }

    return GL_REPEAT;
}

bool isDepthFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::Depth32F:
    case TextureFormat::Depth:
    case TextureFormat::Depth24Stencil8:
        return true;
    default:
        return false;
    }
}

} // namespace zk::OpenGLUtils
