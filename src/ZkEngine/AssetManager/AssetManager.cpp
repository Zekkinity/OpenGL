#include "AssetManager.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Texture2D.hpp"
#include "ZkEngine/Renderer/TextureCube.hpp"
#include "imgui.h"

namespace zk {

AssetManager::AssetManager(const std::filesystem::path& path) : m_AssetRoot(path) {}

AssetManager::~AssetManager() {
    m_LoadedTextures.clear();
    m_LoadedShaders.clear();
}

std::filesystem::path AssetManager::asset(const std::filesystem::path& relative) {
    return m_AssetRoot / relative;
}

// This doesnt check for the texture filtering
std::shared_ptr<Texture> AssetManager::loadTexture(const std::filesystem::path& path, const TextureSpecs& specs) {
    // We stop issues like "../../floorTexture.jpg" and "floorTexture.jpg" from creating two
    // textures
    auto fullPath = std::filesystem::weakly_canonical(asset(path));
    std::string texturePath = fullPath.string();

    // Check if texture is already loaded
    auto it = m_LoadedTextures.find(texturePath);
    if (it != m_LoadedTextures.end()) {
        if (auto existingTexture = it->second.lock()) {
            return existingTexture;
        }
    }

    // Load new texture
    auto texture = Texture2D::load(texturePath, specs);
    m_LoadedTextures.emplace(texturePath, texture);

    return texture;
}

std::shared_ptr<Texture> AssetManager::loadTextureCube(
    const std::vector<std::filesystem::path>& faces) {
    std::vector<std::filesystem::path> paths;
    for (auto& face : faces) {
        auto fullPath = std::filesystem::weakly_canonical(asset(face));
        paths.push_back(fullPath);
    }

    return TextureCube::load(paths);
}

std::shared_ptr<Shader> AssetManager::loadShader(
    std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath) {
    std::string vPath = asset(vertexPath).string();
    std::string fPath = asset(fragmentPath).string();

    std::string gPath;
    if (!geometryPath.empty()) {
        gPath = asset(geometryPath).string();
    }

    std::string key = vPath + "|" + fPath + "|" + gPath;

    // Check if shader is already loaded
    auto it = m_LoadedShaders.find(key);
    if (it != m_LoadedShaders.end()) {
        if (auto existingShader = it->second.lock()) {
            return existingShader;
        }
    }

    // Load new shader
    auto shader = Shader::create(vPath, fPath, gPath);

    m_LoadedShaders.emplace(key, shader);

    return shader;
}

std::shared_ptr<Material> AssetManager::loadMaterial(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> specularMap) {
    m_MaterialCount++;

    return std::make_shared<Material>(shader, texture, specularMap);
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
}

void AssetManager::reloadShaders() {
    int count = 0;

    for (auto& [name, weakShader] : m_LoadedShaders) {
        if (auto shader = weakShader.lock()) {
            shader->reload();
            count++;
        }
    }

    ZK_CORE_DEBUG("Reloaded {} shaders", count);
}

void AssetManager::debug() {
    if (ImGui::Button("Reload Shaders")) {
        reloadShaders();
    }

    ImGui::Text("Loaded Shaders: %zu", m_LoadedShaders.size());
    ImGui::Text("Loaded Textures: %zu", m_LoadedTextures.size());
    ImGui::Text("Loaded Materials: %d", m_MaterialCount);
}

} // namespace zk
