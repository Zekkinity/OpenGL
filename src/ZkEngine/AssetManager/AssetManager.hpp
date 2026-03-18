#pragma once

#include "ZkEngine/Renderer/Texture.hpp"

#include <filesystem>
#include <unordered_map>
#include <vector>

// TODO:
// Make something like this
// AssetManager::get<zk::Shader>("lighting");
// AssetManager::get<zk::Shader>(SHADER::LIGHTING);
// AssetManager::get<zk::Shader>(SHADER_LIGHTING);
// zk::AssetManager::load<zk::Shader>("lighting");
// zk::AssetManager::load<zk::Texture>("floorTexture");
// zk::AssetManager::LoadTexture("floorTexture");
// Use Asset Handles
// use weak pointers as keys
// - Prefer using find() instead of contains and at because it's more efficient

namespace zk {

class Material;
class Shader;

class AssetManager {
  public:
    AssetManager(const std::filesystem::path& path);
    ~AssetManager();

    std::filesystem::path asset(const std::filesystem::path& path);

    // template <typename T>
    // std::shared_ptr<T> load(const std::string& id) {
    //     auto& storage = getStorage<T>();
    //     if (storage.contains(id)) {
    //         return storage.at(id);
    //     } else {
    //         storage.emplace(id, std::make_shared<T>());
    //         return storage.at(id);
    //     }
    //
    //     return std::make_shared<T>();
    // }

    // std::unordered_map<AssetHandle, std::shared_ptr<Asset>> m_Assets;

    // template <typename T>
    // std::unordered_map<std::string, T>& getStorage();
    std::shared_ptr<Texture> loadTexture(const std::filesystem::path& path, const TextureSpecs& specs = {});

    std::shared_ptr<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces);

    std::shared_ptr<Material> loadMaterial(
        std::shared_ptr<Shader> shader,
        std::shared_ptr<Texture> texture = nullptr,
        std::shared_ptr<Texture> specularMap = nullptr);

    std::shared_ptr<Shader> loadShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "");

    void clearUnused();
    void reloadShaders();

    void debug();

  private:
    std::filesystem::path m_AssetRoot;

    std::unordered_map<std::string, std::weak_ptr<Texture>> m_LoadedTextures;
    std::unordered_map<std::string, std::weak_ptr<Shader>> m_LoadedShaders;
    int m_MaterialCount = 0;
};

} // namespace zk
