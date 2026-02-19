#pragma once

#include <vector>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/PhysicsSystem.hpp"
#include "ZkEngine/Scene/LightSystem.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class Entity;

using EntityID = uint32_t;

class Scene {
  public:
    Scene();
    virtual ~Scene();

    virtual void init();
    virtual void update(float dt);

    virtual void submit(Core::RendererBackend& renderer) const;

    Entity createEntity();
    void destroyEntity(EntityID id);

    template <typename T, typename... Args>
    T& addComponent(EntityID id, Args&&... args) {
        auto& storage = getStorage<T>();
        return storage.emplace(id, T(std::forward<Args>(args)...)).first->second;
    }

    template <typename T>
    const T& getComponent(EntityID id) const {
        const auto& storage = getStorage<T>();
        auto it = storage.find(id);
        assert(it != storage.end() && "Component does not exist");
        return it->second;
    }

    template <typename T>
    T& getComponent(EntityID id) {
        auto& storage = getStorage<T>();
        auto it = storage.find(id);
        assert(it != storage.end() && "Component does not exist");
        return it->second;
    }

    template <typename T>
    bool hasComponent(EntityID id) const {
        auto& storage = getStorage<T>();
        return storage.find(id) != storage.end();
    }

    template <typename T>
    const std::unordered_map<EntityID, T>& getStorage() const;
    template <typename T>
    std::unordered_map<EntityID, T>& getStorage();

    void addRenderObject(std::unique_ptr<RenderObject> renderObject);
    void clearObjects();

    Camera3D* getCamera() { return m_Camera.get(); }
    LightSystem& getLightSystem() { return m_LightSystem; }

    virtual void debug();

  protected:
    std::vector<std::unique_ptr<RenderObject>> m_Objects;
    std::unique_ptr<Camera3D> m_Camera;
    PhysicsSystem m_PhysicsSystem;
    LightSystem m_LightSystem;

    EntityID m_NextEntityID = 0;
    std::unordered_map<EntityID, Transform3D> m_Transforms;
    std::unordered_map<EntityID, RenderObject> m_Renderers;

    // Lights
    std::unordered_map<EntityID, DirectionalLight> m_DirectionalLights;
    std::unordered_map<EntityID, SpotLight> m_SpotLights;
    std::unordered_map<EntityID, PointLight> m_PointLights;
};

template <>
inline const std::unordered_map<EntityID, Transform3D>& Scene::getStorage<Transform3D>() const {
    return m_Transforms;
}

template <>
inline const std::unordered_map<EntityID, RenderObject>& Scene::getStorage<RenderObject>() const {
    return m_Renderers;
}

template <>
inline std::unordered_map<EntityID, Transform3D>& Scene::getStorage<Transform3D>() {
    return m_Transforms;
}

template <>
inline std::unordered_map<EntityID, RenderObject>& Scene::getStorage<RenderObject>() {
    return m_Renderers;
}

// Lights
template <>
inline const std::unordered_map<EntityID, DirectionalLight>& Scene::getStorage<DirectionalLight>()
    const {
    return m_DirectionalLights;
}

template <>
inline const std::unordered_map<EntityID, SpotLight>& Scene::getStorage<SpotLight>() const {
    return m_SpotLights;
}

template <>
inline const std::unordered_map<EntityID, PointLight>& Scene::getStorage<PointLight>() const {
    return m_PointLights;
}

template <>
inline std::unordered_map<EntityID, DirectionalLight>& Scene::getStorage<DirectionalLight>() {
    return m_DirectionalLights;
}

template <>
inline std::unordered_map<EntityID, SpotLight>& Scene::getStorage<SpotLight>() {
    return m_SpotLights;
}

template <>
inline std::unordered_map<EntityID, PointLight>& Scene::getStorage<PointLight>() {
    return m_PointLights;
}

} // namespace zk
