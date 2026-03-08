#pragma once

#include <filesystem>
#include <assimp/scene.h>
#include <functional>

#include "ZkEngine/Renderer/API/OpenGL/Mesh.hpp"
#include "ZkEngine/Renderer/Material.hpp"

namespace zk {

struct MeshData {
    std::shared_ptr<Mesh> mesh;
    glm::vec3 diffuseColor;
    std::shared_ptr<Material> material;
};

class Model {
  public:
    Model(const std::filesystem::path& path, std::shared_ptr<Shader> shader);

    const std::vector<MeshData>& getMeshes() const;

  private:
    void loadModel(const std::filesystem::path& path);
    void processNode(aiNode* node, const aiScene* scene);
    MeshData processMesh(aiMesh* mesh, const aiScene* scene);

    // std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);
    std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);

    void loadTextureType(
        aiMaterial* aiMaterial,
        aiTextureType type,
        std::function<void(std::shared_ptr<Texture>)> setter);

  private:
    std::vector<MeshData> m_Meshes;

    std::shared_ptr<Shader> m_Shader;
    std::filesystem::path m_Directory;

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesLoaded;
    std::unordered_map<uint32_t, std::shared_ptr<Material>> m_MaterialsLoaded;
};

} // namespace zk
