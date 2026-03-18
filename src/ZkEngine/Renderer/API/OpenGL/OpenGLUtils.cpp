#include "ZkEngine/Renderer/API/OpenGL/OpenGLUtils.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk::OpenGLUtils {

uint32_t textureInternalFormatToGL(TextureFormat format) {
    switch (format) {
    case TextureFormat::RGB8:
        return GL_RGB8;
    case TextureFormat::RGBA8:
        return GL_RGBA8;
    case TextureFormat::SRGB:
        return GL_SRGB_ALPHA;
    case TextureFormat::Depth:
        return GL_DEPTH_COMPONENT24;
    case TextureFormat::Depth32F:
        return GL_DEPTH_COMPONENT32F;
    case TextureFormat::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    }

    return 0;
}

uint32_t textureFormatToGL(TextureFormat format) {
    switch (format) {
    case TextureFormat::RGB8:
        return GL_RGB;
    case TextureFormat::SRGB:
    case TextureFormat::RGBA8:
        return GL_RGBA;
    case TextureFormat::Depth32F:
    case TextureFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case TextureFormat::Depth24Stencil8:
        return GL_DEPTH_STENCIL;
    }

    return 0;
}

uint32_t textureTypeToGL(TextureFormat format) {
    switch (format) {
    case TextureFormat::Depth:
        return GL_UNSIGNED_INT;

    case TextureFormat::Depth24Stencil8:
        return GL_UNSIGNED_INT_24_8;

    case TextureFormat::Depth32F:
        return GL_FLOAT;

    default:
        return GL_UNSIGNED_BYTE;
    }
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
