#pragma once

#include <filesystem>
#include <assimp/scene.h>
#include <functional>

#include "ZkEngine/Renderer/Material.hpp"
#include "ZkEngine/Renderer/Mesh.hpp"

namespace zk {

struct ModelMeshData {
    Ref<Mesh> mesh;
    Ref<Material> material;
    glm::vec3 diffuseColor;
};

class Entity;
class RenderDevice;

class Model {
  public:
    Model(RenderDevice& device, const std::filesystem::path& path);

    const std::vector<ModelMeshData>& getMeshes() const { return m_Meshes; }

    glm::mat4 getTransform() const { return m_ModelTransform; }

    // angle is in degrees
    void rotate(float angle, glm::vec3 axis) { m_ModelTransform = glm::rotate(m_ModelTransform, glm::radians(angle), axis); }

    const AABB& getBounds() const { return m_Bounds; }

  private:
    void loadModel(const std::filesystem::path& path);
    void processNode(aiNode* node, const aiScene* scene);
    ModelMeshData processMesh(aiMesh* mesh, const aiScene* scene);

    // std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);
    std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);

    void loadTextureType(
        aiMaterial* aiMaterial, aiTextureType type, std::function<void(Ref<Texture>)> setter);

    void computeBounds();

  private:
    std::vector<ModelMeshData> m_Meshes;

    std::filesystem::path m_Directory;

    std::unordered_map<uint32_t, Ref<Material>> m_MaterialsLoaded;

    glm::mat4 m_ModelTransform = glm::mat4(1.0f);

    AABB m_Bounds;

    RenderDevice& m_Device;
};

} // namespace zk
