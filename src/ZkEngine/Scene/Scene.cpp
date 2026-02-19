#include "Scene.hpp"

#include "ZkEngine/Scene/Entity.hpp"

namespace zk {

Scene::Scene() : m_LightSystem(*this) {
    m_Camera = std::make_unique<Camera3D>();
}

Scene::~Scene() {
    m_Renderers.clear();
    m_Transforms.clear();
}

void Scene::init() {}

void Scene::update(float dt) {}

Entity Scene::createEntity() {
    uint32_t id = m_NextEntityID++;
    Entity entity(id, this);
    return entity;
}

void Scene::destroyEntity(EntityID id) {
    m_Transforms.erase(id);
    m_Renderers.erase(id);
}

void Scene::addRenderObject(std::unique_ptr<RenderObject> renderObject) {
    m_Objects.push_back(std::move(renderObject));
}

void Scene::clearObjects() {
    m_Objects.clear();
}

// void Scene::submit(Core::RendererBackend& renderer) const {
//     for (auto& [entity, renderObject] : m_Renderers) {
//         renderer.submit(renderObject, *m_Camera);
//     }
// }

void Scene::submit(Core::RendererBackend& renderer) const {
    for (auto& [entity, renderObject] : m_Renderers) {
        if (!hasComponent<Transform3D>(entity)) {
            continue;
        }

        auto& transform = getComponent<Transform3D>(entity);

        renderer.submit(renderObject, transform);
    }
}

void Scene::debug() {}

} // namespace zk
