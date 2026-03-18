#include "Model.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/Texture2D.hpp"

#include <assimp/Importer.hpp>
#include <filesystem>
#include <assimp/postprocess.h>

namespace zk {

// Model::Model(const std::filesystem::path& path) {
//     loadModel(path);
// }

Model::Model(const std::filesystem::path& path, std::shared_ptr<Shader> shader) : m_Shader(shader) {
    loadModel(path);
}

void Model::loadModel(const std::filesystem::path& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->mRootNode) {
        ZK_CORE_ERROR("Assimp failed to load model: {}", importer.GetErrorString());
        return;
    }

    m_Directory = path.parent_path().string();
    ZK_CORE_DEBUG("Model directory: {}", m_Directory.c_str());

    auto start = std::chrono::high_resolution_clock::now();
    processNode(scene->mRootNode, scene);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    ZK_CORE_DEBUG("Model loaded in {}ms", duration.count());
    // Without Cache: ~24,500ms
    // With Cache: ~1,700ms
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.emplace_back(processMesh(mesh, scene));
    }

    // recurse
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

MeshData Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z,
        };

        if (mesh->HasNormals()) {
            v.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z,
            };
        }
        else {
            v.normal = glm::vec3(0.0f);
        }

        if (mesh->mTextureCoords[0]) {
            v.texCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y,
            };
        }
        else {
            v.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(v);
    }

    // indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Material
    MeshData meshData;
    meshData.mesh = std::make_shared<Mesh>(vertices, indices);
    meshData.diffuseColor = glm::vec3(1.0f);

    if (mesh->mMaterialIndex >= 0) {
        uint32_t materialIndex = mesh->mMaterialIndex;

        // Check if material is already loaded
        if (m_MaterialsLoaded.contains(materialIndex)) {
            // std::cout << "Material already loaded" << std::endl;
            meshData.material = m_MaterialsLoaded.at(materialIndex);
        }

        // Load new material
        else {
            auto material = std::make_shared<Material>(m_Shader);
            aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

            loadTextureType(
                aiMaterial, aiTextureType_DIFFUSE, [&](std::shared_ptr<Texture> texture) {
                    material->setDiffuse(texture);
                });

            loadTextureType(
                aiMaterial, aiTextureType_SPECULAR, [&](std::shared_ptr<Texture> texture) {
                    material->setSpecular(texture);
                });

            aiColor3D color;
            aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            meshData.diffuseColor = glm::vec3(color.r, color.g, color.b);

            float shininess = 0.0f;
            if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
                material->setShininess(shininess);
            }

            m_MaterialsLoaded.emplace(materialIndex, material);
            meshData.material = material;
        }
    }

    return meshData;
}

void Model::loadTextureType(
    aiMaterial* aiMaterial,
    aiTextureType type,
    std::function<void(std::shared_ptr<Texture>)> setter) {
    // std::cout << "Diffuse count: " << material->GetTextureCount(aiTextureType_DIFFUSE) <<
    // std::endl;

    for (uint32_t i = 0; i < aiMaterial->GetTextureCount(type); i++) {
        aiString str;
        if (aiMaterial->GetTexture(type, i, &str) != AI_SUCCESS) {
            continue;
        };

        const std::string texturePath = m_Directory / str.C_Str();
        // std::cout << "texture path: " << texturePath << std::endl;

        if (texturePath.empty())
            return;

        // Check if texture is already loaded
        auto it = m_TexturesLoaded.find(texturePath);
        if (it != m_TexturesLoaded.end()) {
            setter(it->second);
        }
        // Load new texture
        else {
            ZK_CORE_DEBUG("Loading texture: {}", texturePath);
            auto texture = Texture2D::load(texturePath);
            m_TexturesLoaded.emplace(texturePath, texture);
            setter(texture);
        }
    }
}

const std::vector<MeshData>& Model::getMeshes() const {
    return m_Meshes;
}

} // namespace zk
