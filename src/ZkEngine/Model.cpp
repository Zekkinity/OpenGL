#include "Model.hpp"

#include <assimp/Importer.hpp>
#include <iostream>
#include <assimp/postprocess.h>

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs |
                                    aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }

    m_Directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
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
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
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
        } else {
            v.normal = glm::vec3(0.0f);
        }

        if (mesh->mTextureCoords[0]) {
            v.texCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y,
            };
        } else {
            v.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(v);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Material
    MeshData meshData;
    meshData.mesh = Mesh(vertices, indices);
    meshData.diffuseColor = glm::vec3(1.0f);

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::string diffuseTexturePath = loadMaterialTexture(material, aiTextureType_DIFFUSE);
        if (!diffuseTexturePath.empty()) {
            meshData.diffuseTexturePath = diffuseTexturePath;
        }

        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        meshData.diffuseColor = glm::vec3(color.r, color.g, color.b);
    }

    return meshData;
}

std::string Model::loadMaterialTexture(aiMaterial* material, aiTextureType type) {
    if (material->GetTextureCount(type) > 0) {
        aiString str;
        material->GetTexture(type, 0, &str);
        std::string texturePath = m_Directory + '/' + str.C_Str();
        // std::cout << "texture path: " << texturePath << std::endl;
        return texturePath;
    } else {
        // std::cout << "no texture" << std::endl;
    }
    return "";
}

const std::vector<MeshData>& Model::getMeshes() const {
    return m_Meshes;
}
