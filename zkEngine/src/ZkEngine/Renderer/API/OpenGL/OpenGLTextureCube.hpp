#pragma once

#include "ZkEngine/Renderer/Texture.hpp"
#include "ZkEngine/Utils/ImageLoader.hpp"

namespace zk {

class OpenGLTextureCube : public Texture {
  public:
    OpenGLTextureCube() = delete;

    OpenGLTextureCube(const TextureSpecs& specs, const std::array<ImageData, 6>& images = {});

    ~OpenGLTextureCube();

    void bind(uint32_t slot) const override;
    void unbind(uint32_t slot) const override;

    int getWidth() const override { return m_Specs.width; }
    int getHeight() const override { return m_Specs.height; }

  private:
    uint32_t m_RendererID = 0;
    TextureSpecs m_Specs;
};

} // namespace zk
