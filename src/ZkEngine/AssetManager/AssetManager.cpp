#include "AssetManager.hpp"

#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/Shader.hpp"

namespace zk {

AssetManager::AssetManager(const std::filesystem::path& path) : m_AssetRoot(path) {}

AssetManager::~AssetManager() {
    m_LoadedMaterials.clear();
    m_LoadedTextures.clear();
    m_LoadedShaders.clear();
}

std::filesystem::path AssetManager::asset(const std::filesystem::path& relative) {
    return m_AssetRoot / relative;
}

// This doesnt check for the texture filtering
std::shared_ptr<Texture> AssetManager::loadTexture(
    const std::filesystem::path& path, TextureFiltering filtering) {
    std::string texturePath = asset(path).string();

    // Check if texture is already loaded
    auto it = m_LoadedTextures.find(texturePath);
    if (it != m_LoadedTextures.end()) {
        if (auto existingTexture = it->second.lock()) {
            return existingTexture;
        }
    }

    // Load new texture
    auto texture = Texture::create(texturePath, filtering);
    m_LoadedTextures.emplace(texturePath, texture);

    return texture;
}

std::shared_ptr<Shader> AssetManager::loadShader(
    const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    std::string vPath = asset(vertexPath).string();
    std::string fPath = asset(fragmentPath).string();

    std::string key = vPath + "|" + fPath;

    // Check if shader is already loaded
    // auto it = m_LoadedShaders.find(key);
    // if (it != m_LoadedShaders.end()) {
    //     if (auto existingShader = it->second.lock()) {
    //         return existingShader;
    //     }
    // }

    // Load new shader
    auto shader = Shader::create(vPath, fPath);

    // m_LoadedShaders.emplace(key, shader);

    return shader;
}

// FIX: This function will always load the same shader for all materials
std::shared_ptr<Material> AssetManager::loadMaterial(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> specularMap) {
    // Check if material is already loaded
    // auto it = m_LoadedMaterials.find();
    // if (it != m_LoadedMaterials.end()) {
    //     if (auto existingMaterial = it->second.lock()) {
    //         return existingMaterial;
    //     }
    // }

    // auto shader = loadShader(materialName);

    auto material = std::make_shared<Material>(shader, texture, specularMap);
    // m_LoadedMaterials.emplace(materialName, material);
    return material;
}

void AssetManager::clearUnused() {
    auto clearMap = [](auto& map) {
        for (auto it = map.begin(); it != map.end();) {
            if (it->second.expired())
                it = map.erase(it);
            else
                ++it;
        }
    };

    clearMap(m_LoadedTextures);
    clearMap(m_LoadedShaders);
    clearMap(m_LoadedMaterials);
}

void AssetManager::reloadShader(const std::string& name) {
    auto it = m_LoadedShaders.find(name);
    if (it == m_LoadedShaders.end())
        return;

    if (auto shader = it->second.lock()) {
        // shader->reload();
    }
}

} // namespace zk
