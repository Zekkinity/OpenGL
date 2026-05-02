#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

namespace zk::OpenGLUtils {

uint32_t textureFormatToGL(TextureFormat format);
uint32_t textureInternalFormatToGL(TextureFormat format);
uint32_t textureTypeToGL(TextureFormat format);

uint32_t textureFilterToGL(TextureFilter filter);
uint32_t textureWrapToGL(TextureWrap wrap);

bool isDepthFormat(TextureFormat format);

} // namespace zk::OpenGLUtils
