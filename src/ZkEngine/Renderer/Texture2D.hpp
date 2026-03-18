#pragma once

#include "ZkEngine/Renderer/Texture.hpp"
#include <filesystem>

namespace zk {

class Texture2D : public Texture {
  public:
    static std::shared_ptr<Texture2D> load(const std::filesystem::path& path, const TextureSpecs& specs = {});
    static std::shared_ptr<Texture2D> create(const TextureSpecs& specs);
};

} // namespace zk
