#include "ModelObject.hpp"

namespace zk {

ModelObject::ModelObject(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
    : m_Model(model) {
    const auto& meshes = model->getMeshes();
    for (size_t i = 0; i < meshes.size(); i++) {
        const auto& meshData = meshes[i];

        // Create texture if the mesh has one
        std::shared_ptr<Texture> texture;
        if (!meshData.diffuseTexturePath.empty()) {
            texture = std::make_shared<Texture>(meshData.diffuseTexturePath);
            m_Textures.push_back(texture);
        }

        // Create material for this mesh

        auto material = std::make_shared<Material>(shader, texture);
        m_Materials.push_back(material);

        // Store mesh index instead of pointer
        m_MeshIndices.push_back(i);

        // Store color
        m_Colors.push_back(glm::vec4(meshData.diffuseColor, 1.0f));
        m_HasTexture.push_back(texture != nullptr);
    }
}

ModelObject::~ModelObject() {}

void ModelObject::draw(const glm::mat4& viewProj) const {
    const auto& meshes = m_Model->getMeshes();

    for (size_t i = 0; i < m_MeshIndices.size(); i++) {
        const auto& meshData = meshes[m_MeshIndices[i]];
        std::shared_ptr<Material> material = m_Materials[i];

        material->bind(viewProj, transform.getMatrix());

        // Set color if no texture or for tinting
        if (!m_HasTexture[i]) {
            material->getShader().setUniform4fv("u_Color", m_Colors[i]);
        }

        meshData.mesh.bind();
        glDrawElements(
            GL_TRIANGLES, meshData.mesh.getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
    }
}

} // namespace zk
