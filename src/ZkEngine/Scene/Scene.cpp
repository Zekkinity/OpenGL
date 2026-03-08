#include "Scene.hpp"
#include "ZkEngine/Model.hpp"

#include "ZkEngine/Core/Base.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace zk {

Scene::Scene() : m_LightSystem(m_Registry) {
    m_Camera = std::make_unique<Camera3D>();
}

Scene::~Scene() {}

void Scene::init() {}

// Manage all rigid bodies
void Scene::update(float dt) {
    for (auto& [entity, rigidBody] : m_Registry.getStorage<RigidBody>()) {
        if (!m_Registry.hasComponent<Transform3D>(entity)) {
            continue;
        }

        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        // rigidBody.update(dt);
        transform.position = rigidBody.position;
        transform.rotation = rigidBody.rotation;
    }

    // m_PhysicsSystem.update(m_Registry, *m_Camera, dt);
}

// Manage all renderable objects
void Scene::submit(RendererBackend& renderer) const {
    for (auto& [entity, renderObject] : m_Registry.getStorage<RenderObject>()) {
        if (!m_Registry.hasComponent<Transform3D>(entity)) {
            ZK_CORE_WARN("No transform");
            continue;
        }

        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        RenderCommand cmd;
        cmd.mesh = renderObject.mesh.get();
        cmd.material = renderObject.material.get();
        cmd.modelMatrix = transform.getMatrix();
        cmd.distanceToCamera = glm::length2(m_Camera->getPosition() - transform.position);
        renderer.submit(cmd);
    }

    for (auto& [entity, model] : m_Registry.getStorage<Model>()) {
        if (!m_Registry.hasComponent<Transform3D>(entity)) {
            continue;
        }
        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        RenderCommand cmd;
        for (auto& mesh : model.getMeshes()) {
            cmd.mesh = mesh.mesh.get();
            cmd.material = mesh.material.get();
            cmd.modelMatrix = transform.getMatrix();

            renderer.submit(cmd);
        }
    }
}

void Scene::debug() {}

} // namespace zk
