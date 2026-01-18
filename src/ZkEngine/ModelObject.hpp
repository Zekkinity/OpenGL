#pragma once

#include "Renderer/Renderer.hpp"

#include "Model.hpp"
#include "Material.hpp"
#include "Transform3D.hpp"

class ModelObject {
   public:
    ModelObject(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader);
    ~ModelObject();

    void draw(const glm::mat4& viewProj) const;

    Transform3D transform;

   private:
    std::shared_ptr<Model> m_Model;
    std::vector<size_t> m_MeshIndices;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    std::vector<std::shared_ptr<Material>> m_Materials;
    std::vector<glm::vec4> m_Colors;
    std::vector<bool> m_HasTexture;
};
