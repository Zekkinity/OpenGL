#include "zkpch.hpp"
#include "Model.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Vertex.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <filesystem>

namespace zk {

Model::Model(RenderDevice& device, const std::filesystem::path& path) : m_Device(device) {
    loadModel(path);

    computeBounds();
}

void Model::loadModel(const std::filesystem::path& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_GenNormals | // aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

    if (!scene || !scene->mRootNode) {
        ZK_CORE_ERROR("Assimp failed to load model: {}", importer.GetErrorString());
        return;
    }

    m_Directory = path.parent_path().string();
    // ZK_CORE_DEBUG("Model directory: {}", m_Directory.c_str());
    m_Meshes.reserve(scene->mNumMeshes);

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

ModelMeshData Model::processMesh(aiMesh* mesh, const aiScene* scene) {
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

        if (mesh->HasTangentsAndBitangents()) {
            v.tangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z,
            };
            v.bitangent = {
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z,
            };
        }
        else {
            v.tangent = glm::vec3(0.0f);
            v.bitangent = glm::vec3(0.0f);
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
    ModelMeshData meshData;
    meshData.mesh = m_Device.createMesh(vertices, indices);
    // meshData.mesh->setAABB(mesh->mAABB);
    meshData.diffuseColor = glm::vec3(1.0f);

    if (mesh->mMaterialIndex >= 0) {
        uint32_t materialIndex = mesh->mMaterialIndex;

        // reuse if already loaded for this model
        if (m_MaterialsLoaded.contains(materialIndex)) {
            meshData.material = m_MaterialsLoaded.at(materialIndex);
        }
        else {
            auto material = Asset::loadMaterial({});
            aiMaterial* aiMaterial = scene->mMaterials[materialIndex];

            // loadTextureType(aiMaterial, aiTextureType_BASE_COLOR, [&](Ref<Texture> texture) {
            //     ZK_CORE_DEBUG("Loaded Base Color: ");
            //     material->setDiffuse(texture);
            // });
            loadTextureType(aiMaterial, aiTextureType_DIFFUSE, [&](Ref<Texture> texture) {
                // ZK_CORE_DEBUG("Loaded Diffuse: ");
                material->setDiffuse(texture);
            });

            loadTextureType(aiMaterial, aiTextureType_SPECULAR, [&](Ref<Texture> texture) {
                // ZK_CORE_DEBUG("Loaded Specular: ");
                material->setSpecular(texture);
            });

            // Doesnt work for .obj
            loadTextureType(aiMaterial, aiTextureType_NORMALS, [&](Ref<Texture> texture) {
                // ZK_CORE_DEBUG("Loaded Normal: ");
                material->setNormal(texture);
            });

            // GLTF
            loadTextureType(
                aiMaterial, aiTextureType_GLTF_METALLIC_ROUGHNESS, [&](Ref<Texture> texture) {
                    // ZK_CORE_DEBUG("Loaded Metallic Roughness");
                    material->setMetallicRoughness(texture);
                });

            loadTextureType(aiMaterial, aiTextureType_AMBIENT_OCCLUSION, [&](Ref<Texture> texture) {
                // ZK_CORE_DEBUG("Loaded Ambient Occlusion");
                material->setAmbientOcclusion(texture);
            });

            aiColor3D color;
            aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            meshData.diffuseColor = glm::vec3(color.r, color.g, color.b);

            float shininess = 0.0f;
            if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
                material->setShininess(shininess);

            m_MaterialsLoaded.emplace(materialIndex, material);
            meshData.material = material;
        }
    }

    return meshData;
}

void Model::loadTextureType(
    aiMaterial* aiMaterial, aiTextureType type, std::function<void(Ref<Texture>)> setter) {
    // std::cout << "Diffuse count: " << material->GetTextureCount(aiTextureType_DIFFUSE) <<
    // std::endl;

    for (uint32_t i = 0; i < aiMaterial->GetTextureCount(type); i++) {
        aiString str;
        if (aiMaterial->GetTexture(type, i, &str) != AI_SUCCESS) {
            continue;
        };
        const std::string texturePath = m_Directory / str.C_Str();

        // ZK_CORE_DEBUG("Loading texture: {}", texturePath);
        if (texturePath.empty())
            return;

        Ref<Texture> texture;
        texture = Asset::loadTexture(texturePath);

        setter(texture);
    }
}

void Model::computeBounds() {
    for (size_t i = 0; i < m_Meshes.size(); i++) {
        auto& meshBounds = m_Meshes[i].mesh->getAABB();
        if (i == 0) {
            m_Bounds.min = meshBounds.min;
            m_Bounds.max = meshBounds.max;
        }
        else {
            m_Bounds.min = glm::min(m_Bounds.min, meshBounds.min);
        }
        m_Bounds.max = glm::max(m_Bounds.max, meshBounds.max);
    }
}

} // namespace zk
