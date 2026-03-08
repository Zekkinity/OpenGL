#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

#include <cstdint>
#include <string>

namespace zk {

class OpenGLTexture : public Texture {
  public:
    static std::shared_ptr<OpenGLTexture> create(
        const std::string& path, TextureFiltering filtering);

    ~OpenGLTexture();

    void bind(unsigned int slot = 0) const override;
    void unbind() const override;

    int getWidth() const override { return m_Width; }
    int getHeight() const override { return m_Height; }

  private:
    OpenGLTexture() = delete;
    OpenGLTexture(const std::string& path, TextureFiltering filtering);

  private:
    uint32_t m_RendererID = 0;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer = nullptr;
    int m_Width = 0;
    int m_Height = 0;
    int m_BPP = 0; // bytes per pixel
};

} // namespace zk
