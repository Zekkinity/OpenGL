#pragma once

#include <assimp/scene.h>

#include "Mesh.hpp"

namespace zk {

struct MeshData {
    Mesh mesh;
    std::string diffuseTexturePath;
    glm::vec3 diffuseColor;
};

class Model {
  public:
    Model(const std::string& path);

    const std::vector<MeshData>& getMeshes() const;

  private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    MeshData processMesh(aiMesh* mesh, const aiScene* scene);

    std::string loadMaterialTexture(aiMaterial* material, aiTextureType type);

  private:
    // std::vector<Mesh> m_Meshes;
    std::vector<MeshData> m_Meshes;
    std::string m_Directory;
};

} // namespace zk
