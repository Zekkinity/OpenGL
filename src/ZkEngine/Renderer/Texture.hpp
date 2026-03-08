#pragma once

#include <memory>

namespace zk {

enum TextureFiltering { NEAREST = 0x2600, LINEAR = 0x2601 };

class Texture {
  protected:
    Texture() = default;
    virtual ~Texture() = default;

  public:
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    virtual void bind(unsigned int slot = 0) const = 0;
    virtual void unbind() const = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    static std::shared_ptr<Texture> create(
        const std::string& path, TextureFiltering filtering = TextureFiltering::NEAREST);
};

} // namespace zk
