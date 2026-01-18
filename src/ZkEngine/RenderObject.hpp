#pragma once

#include "Mesh.hpp"
#include "Material.hpp"
#include "Transform3D.hpp"

class RenderObject {
   public:
    RenderObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : mesh(mesh), material(material) {}

    Transform3D transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    glm::vec4 color = {1, 1, 1, 1};
    bool colorOverride = false;

    void setColor(const glm::vec4& color) {
        this->color = color;
        colorOverride = true;
    }
};
