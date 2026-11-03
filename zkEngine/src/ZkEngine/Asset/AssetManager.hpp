#pragma once

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Texture.hpp"
#include <unordered_map>

namespace zk {

enum class PrimitiveType { Cube, Sphere, Quad, Line, Triangle, Grid };

static std::string primitiveToString(PrimitiveType type) {
    switch (type) {
    case PrimitiveType::Cube:
        return "Cube";
    case PrimitiveType::Sphere:
        return "Sphere";
    case PrimitiveType::Quad:
        return "Quad";
    case PrimitiveType::Line:
        return "Line";
    case PrimitiveType::Triangle:
        return "Triangle";
    case PrimitiveType::Grid:
        return "Grid";
    default:
        return "";
    }
}

class Shader;

class AssetManager {
  public:
    AssetManager() = delete;
    AssetManager(RenderDevice& device, const std::filesystem::path& root);
    ~AssetManager();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

  public:
    Ref<Texture> loadTexture(const std::filesystem::path& path, const TextureSpecs& specs = {});

    Ref<Texture> loadTextureHDR(const std::string& path);

    Ref<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces);

    Ref<Material> loadMaterial(const MaterialSpecs& specs);

    Ref<Shader> loadShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "");

    Ref<Model> loadModel(const std::filesystem::path& path);

    Ref<Mesh> getMesh(PrimitiveType type);

    Ref<Texture> getWhiteTexture() const { return m_WhiteTexture; }
    Ref<Texture> getBlackTexture() const { return m_BlackTexture; }
    Ref<Texture> getDefaultNormal() const { return m_DefaultNormal; }

    void clearUnused();
    void reloadShaders();

    void debug();

  private:
    std::string normalizePath(const std::filesystem::path& path);
    std::string cleanPath(std::string path);

  private:
    RenderDevice& m_Device;

    std::filesystem::path m_AssetRoot;

    // std::unordered_map<AssetHandle, Ref<Asset>> m_Assets;
    std::unordered_map<std::string, std::weak_ptr<Texture>> m_LoadedTextures;
    std::unordered_map<std::string, std::weak_ptr<Shader>> m_LoadedShaders;
    int m_MaterialCount = 0;

    std::unordered_map<std::string, std::weak_ptr<Model>> m_LoadedModels;

    std::unordered_map<std::string, std::weak_ptr<Mesh>> m_LoadedMeshes;

    Ref<Texture> m_WhiteTexture;
    Ref<Texture> m_BlackTexture;
    Ref<Texture> m_DefaultNormal;
};

} // namespace zk
