#pragma once

#include <cstdint>
#include <string>

namespace zk {

enum TextureFiltering { NEAREST = 0x2600, LINEAR = 0x2601 };

class Texture {
  public:
    Texture(const std::string& path, TextureFiltering filtering);
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

  private:
    uint32_t m_RendererID = 0;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer = nullptr;
    int m_Width = 0;
    int m_Height = 0;
    int m_BPP = 0; // bytes per pixel
};

} // namespace zk
