#pragma once

#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/Model.hpp"

#include "ZkEngine/World/Lights.hpp"
#include "ZkEngine/World/Transform3D.hpp"

namespace zk {

struct ModelComponent {
    Ref<Model> model;
    bool uploaded = false;
};

struct RenderObject {
    Ref<Mesh> mesh;
    Ref<Material> material;

    RenderObject(Ref<Mesh> m, Ref<Material> mat) : mesh(std::move(m)), material(std::move(mat)) {
        ZK_CORE_ASSERT(mesh, "RenderObject requires a mesh");
        ZK_CORE_ASSERT(material, "RenderObject requires a material");
    }
};

struct RigidBody {
    glm::vec3 velocity{};
    glm::vec3 acceleration{};

    float drag = 8.0f;

    glm::vec3 rotation{};
    glm::vec3 angularVelocity{};

    float mass = 1.0f;
    bool grounded = false;

    void addForce(const glm::vec3& force) { acceleration += force / mass; }
    void addImpulse(const glm::vec3& impulse) { velocity += impulse / mass; }
};

} // namespace zk
