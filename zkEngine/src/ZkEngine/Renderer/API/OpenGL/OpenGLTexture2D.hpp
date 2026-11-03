#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

#include "ZkEngine/Utils/ImageLoader.hpp"

namespace zk {

class OpenGLTexture2D : public Texture {
  public:
    OpenGLTexture2D() = delete;

    OpenGLTexture2D(const TextureSpecs& specs, ImageData image = {});

    ~OpenGLTexture2D();

    void bind(uint32_t slot) const override;
    void unbind(uint32_t slot) const override;

    int getWidth() const override { return m_Specs.width; }
    int getHeight() const override { return m_Specs.height; }

    void* getData() const { return m_Image.data; }
    int getChannels() const { return m_Image.channels; }

  private:
    uint32_t m_RendererID = 0;

    TextureSpecs m_Specs;
    ImageData m_Image;
};

} // namespace zk
