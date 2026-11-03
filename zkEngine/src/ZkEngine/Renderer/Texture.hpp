#pragma once

#include <cstdint>

namespace zk {

enum class TextureType {
    Texture2D,
    TextureCubeMap,
};

enum class TextureFormat { None = 0, R8, RGB8, RGBA8, RGBA16F, SRGB, Depth24Stencil8, Depth, Depth32F };

enum class TextureFilter { Nearest, Linear };

enum class TextureWrap { ClampToEdge, ClampToBorder, Repeat };

struct TextureSpecs {
    int width = 1;
    int height = 1;

    TextureFormat format = TextureFormat::RGBA8;

    TextureFilter filter = TextureFilter::Linear;
    TextureWrap wrap = TextureWrap::Repeat;
};

class Texture {
  protected:
    Texture() = default;
    virtual ~Texture() = default;

  public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    virtual void bind(uint32_t slot = 0) const = 0;
    virtual void unbind(uint32_t slot = 0) const = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

} // namespace zk
