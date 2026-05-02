#include "zkpch.hpp"
#include "AssetManager.hpp"

#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Texture.hpp"

#include "ZkEngine/Utils/MeshFactory.hpp"
#include "imgui.h"

namespace zk {

/*
    TODO:
    - Implement this for meshes aswell
    Like loadMesh
*/

AssetManager::AssetManager(RenderDevice& device, const std::filesystem::path& root)
    : m_Device(device), m_AssetRoot(root) {
    uint8_t pixel[4] = {255, 255, 255, 255};
    m_WhiteTexture = m_Device.createTexture(
        {},
        {
            .data = pixel,
            .width = 1,
            .height = 1,
            .channels = 4,
        });

    // default Normal map
    unsigned char data[] = {128, 128, 255};
    m_DefaultNormal = m_Device.createTexture(
        {},
        {
            .data = data,
            .width = 1,
            .height = 1,
            .channels = 3,
        });
}

AssetManager::~AssetManager() {
    m_LoadedTextures.clear();
    m_LoadedShaders.clear();

    m_WhiteTexture.reset();
}

// We stop issues like "../../floorTexture.jpg" and "floorTexture.jpg" from creating two
std::string AssetManager::normalizePath(const std::filesystem::path& path) {
    return std::filesystem::weakly_canonical(m_AssetRoot / path).string();
}

// This doesnt check for the texture filtering
Ref<Texture> AssetManager::loadTexture(
    const std::filesystem::path& path, const TextureSpecs& specs) {
    if (path.empty()) {
        ZK_CORE_ERROR("Invalid texture path");
        return nullptr;
    }

    std::string texturePath = normalizePath(path);

    // Check if texture is already loaded
    auto it = m_LoadedTextures.find(texturePath);
    if (it != m_LoadedTextures.end()) {
        if (auto existingTexture = it->second.lock()) {
            return existingTexture;
        }
    }

    // Load new texture
    auto texture = m_Device.loadTexture(texturePath, specs);
    m_LoadedTextures.emplace(texturePath, texture);

    return texture;
}

Ref<Texture> AssetManager::loadTextureCube(const std::vector<std::filesystem::path>& faces) {
    if (faces.empty()) {
        ZK_CORE_ERROR("Invalid texture path");
        return nullptr;
    }

    std::vector<std::filesystem::path> paths;
    for (auto& face : faces) {
        auto fullPath = normalizePath(face);
        paths.push_back(fullPath);
    }

    return m_Device.loadTextureCube(paths);
}

Ref<Shader> AssetManager::loadShader(
    std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath) {
    if (vertexPath.empty() || fragmentPath.empty()) {
        ZK_CORE_ERROR("Invalid shader path");
        return nullptr;
    }

    std::string vPath = normalizePath(vertexPath);
    std::string fPath = normalizePath(fragmentPath);

    std::string gPath;
    if (!geometryPath.empty()) {
        gPath = normalizePath(geometryPath);
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
    auto shader = m_Device.createShader(vPath, fPath, gPath);

    m_LoadedShaders.emplace(key, shader);

    return shader;
}

Ref<Material> AssetManager::loadMaterial(const MaterialSpecs& specs) {
    m_MaterialCount++;

    return createRef<Material>(specs, this);
}


Ref<Model> AssetManager::loadModel(const std::filesystem::path& path) {
    if (path.empty()) {
        ZK_CORE_ERROR("Invalid model path");
        return nullptr;
    }

    std::string key = normalizePath(path);

    auto it = m_LoadedModels.find(key);
    if (it != m_LoadedModels.end()) {
        if (auto existingModel = it->second.lock()) {
            return existingModel;
        }
    }

    auto model = m_Device.loadModel(key);

    m_LoadedModels.emplace(key, model);

    return model;
}

Ref<Mesh> AssetManager::getMesh(PrimitiveType type) {
    std::string key = primitiveToString(type);

    auto it = m_LoadedMeshes.find(key);
    if (it != m_LoadedMeshes.end()) {
        if (auto existingMesh = it->second.lock()) {
            return existingMesh;
        }
    }

    MeshData meshData;
    switch (type) {
    case PrimitiveType::Line:
        meshData = MeshFactory::createLine();
        break;
    case PrimitiveType::Triangle:
        meshData = MeshFactory::createTriangle();
        break;
    case PrimitiveType::Quad:
        meshData = MeshFactory::createQuad();
        break;
    case PrimitiveType::Grid:
        meshData = MeshFactory::createGrid(10);
        break;
    case PrimitiveType::Sphere:
        meshData = MeshFactory::createSphere();
        break;
    case PrimitiveType::Cube:
        meshData = MeshFactory::createCube();
        break;
    }

    auto mesh = m_Device.createMesh(meshData.vertices, meshData.indices);

    m_LoadedMeshes.emplace(key, mesh);

    return mesh;
}

void AssetManager::clearUnused() {
    auto clean = [](auto& map) {
        for (auto it = map.begin(); it != map.end();) {
            if (it->second.expired())
                it = map.erase(it);
            else
                ++it;
        }
    };

    clean(m_LoadedTextures);
    clean(m_LoadedShaders);
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
    ImGui::Text("Loaded Models: %zu", m_LoadedModels.size());
    ImGui::Text("Loaded Meshes: %zu", m_LoadedMeshes.size());

    ImGui::Separator();

    if (ImGui::TreeNode("Shaders")) {
        int count = 0;
        for (auto& [name, weakShader] : m_LoadedShaders) {
            if (auto shader = weakShader.lock()) {
                std::string shaderName = std::to_string(++count) + " - " + cleanPath(name);
                ImGui::Text("%s", shaderName.c_str());
            }
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Textures")) {
        int count = 0;
        for (auto& [name, weakTexture] : m_LoadedTextures) {
            if (auto texture = weakTexture.lock()) {
                std::string textureName = std::to_string(++count) + " - " + cleanPath(name);
                ImGui::Text("%s", textureName.c_str());
            }
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Models")) {
        int count = 0;
        for (auto& [name, weakMaterial] : m_LoadedModels) {
            if (auto material = weakMaterial.lock()) {
                std::string materialName = std::to_string(++count) + " - " + cleanPath(name);
                ImGui::Text("%s", materialName.c_str());
            }
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Meshes")) {
        int count = 0;
        for (auto& [name, weakMesh] : m_LoadedMeshes) {
            if (auto mesh = weakMesh.lock()) {
                std::string meshName = std::to_string(++count) + " - " + cleanPath(name);
                ImGui::Text("%s", meshName.c_str());
            }
        }
        ImGui::TreePop();
    }
}

std::string AssetManager::cleanPath(std::string path) {
    std::string root = std::filesystem::weakly_canonical(m_AssetRoot).string();
    std::string shaders = root + "/shaders/";
    std::string textures = root + "/textures/";
    std::string models = root + "/models/";

    size_t pos;

    // Shaders
    while ((pos = path.find(shaders)) != std::string::npos) {
        path.erase(pos, shaders.length());
    }

    // Textures
    while ((pos = path.find(textures)) != std::string::npos) {
        path.erase(pos, textures.length());
    }

    // Models
    while ((pos = path.find(models)) != std::string::npos) {
        path.erase(pos, models.length());
    }

    return path;
}

} // namespace zk
