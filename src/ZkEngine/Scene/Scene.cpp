#include "Scene.hpp"

namespace zk {

Scene::Scene() {
    m_Camera = std::make_unique<Camera3D>();
}

Scene::~Scene() {}

void Scene::init() {}

void Scene::update(float dt) {}

void Scene::addRenderObject(std::unique_ptr<RenderObject> renderObject) {
    m_Objects.push_back(std::move(renderObject));
}

void Scene::clearObjects() {
    m_Objects.clear();
}

void Scene::submit(Core::RendererBackend& renderer) const {}

void Scene::debug() {}

} // namespace zk
