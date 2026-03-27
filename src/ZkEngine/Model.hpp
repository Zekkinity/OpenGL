#pragma once

#include <filesystem>
#include <assimp/scene.h>
#include <functional>

#include "ZkEngine/ECS/Entity.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"

#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/Vertex.hpp"

namespace zk {

struct ModelMeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::shared_ptr<Material> material;
    glm::vec3 diffuseColor;
};

class Model {
  public:
    Model(const std::filesystem::path& path, std::shared_ptr<Shader> shader);

    void spawn(Entity parent, RenderDevice& renderer) const;

    bool uploaded = false;

  private:
    void loadModel(const std::filesystem::path& path);
    void processNode(aiNode* node, const aiScene* scene);
    ModelMeshData processMesh(aiMesh* mesh, const aiScene* scene);

    // std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);
    std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);

    void loadTextureType(
        aiMaterial* aiMaterial,
        aiTextureType type,
        std::function<void(std::shared_ptr<Texture>)> setter);

  private:
    std::vector<ModelMeshData> m_Meshes;

    std::shared_ptr<Shader> m_Shader;
    std::filesystem::path m_Directory;

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesLoaded;
    std::unordered_map<uint32_t, std::shared_ptr<Material>> m_MaterialsLoaded;
};

} // namespace zk
