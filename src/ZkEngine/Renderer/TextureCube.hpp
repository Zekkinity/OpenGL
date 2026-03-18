#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace zk {

class TextureCube : public Texture {
  public:
    static std::shared_ptr<Texture> load(const std::vector<std::filesystem::path>& faces);
    static std::shared_ptr<Texture> create(const TextureSpecs& specs);
};

} // namespace zk
