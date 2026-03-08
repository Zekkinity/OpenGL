#pragma once

#include "ZkEngine/Physics/RigidBody.hpp"
#include "ZkEngine/Transform3D.hpp"
#include "ZkEngine/Scene/Lights.hpp"

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <utility>

namespace zk {

class Scene;
class Entity;
class RenderObject;
class Model;

using EntityID = uint32_t;

class Registry {
  public:
    Registry();
    ~Registry();

    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    Registry(Registry&&) = delete;
    Registry& operator=(Registry&&) = delete;

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

  private:
    EntityID m_NextEntityID = 0;

    // Use std::vector for better performance (more cache hits)
    std::unordered_map<EntityID, Transform3D> m_Transforms;

    std::unordered_map<EntityID, RenderObject> m_Renderers;
    std::unordered_map<EntityID, Model> m_Models;

    std::unordered_map<EntityID, RigidBody> m_RigidBodies;

    // Lights
    std::unordered_map<EntityID, DirectionalLight> m_DirectionalLights;
    std::unordered_map<EntityID, SpotLight> m_SpotLights;
    std::unordered_map<EntityID, PointLight> m_PointLights;
};

template <>
inline const std::unordered_map<EntityID, Transform3D>& Registry::getStorage<Transform3D>() const {
    return m_Transforms;
}

template <>
inline const std::unordered_map<EntityID, RenderObject>& Registry::getStorage<RenderObject>()
    const {
    return m_Renderers;
}

template <>
inline const std::unordered_map<EntityID, Model>& Registry::getStorage<Model>() const {
    return m_Models;
}

template <>
inline std::unordered_map<EntityID, Model>& Registry::getStorage<Model>() {
    return m_Models;
}

template <>
inline std::unordered_map<EntityID, Transform3D>& Registry::getStorage<Transform3D>() {
    return m_Transforms;
}

template <>
inline std::unordered_map<EntityID, RenderObject>& Registry::getStorage<RenderObject>() {
    return m_Renderers;
}

template <>
inline std::unordered_map<EntityID, RigidBody>& Registry::getStorage<RigidBody>() {
    return m_RigidBodies;
}

// Lights
template <>
inline const std::unordered_map<EntityID, DirectionalLight>&
Registry::getStorage<DirectionalLight>() const {
    return m_DirectionalLights;
}

template <>
inline const std::unordered_map<EntityID, SpotLight>& Registry::getStorage<SpotLight>() const {
    return m_SpotLights;
}

template <>
inline const std::unordered_map<EntityID, PointLight>& Registry::getStorage<PointLight>() const {
    return m_PointLights;
}

template <>
inline std::unordered_map<EntityID, DirectionalLight>& Registry::getStorage<DirectionalLight>() {
    return m_DirectionalLights;
}

template <>
inline std::unordered_map<EntityID, SpotLight>& Registry::getStorage<SpotLight>() {
    return m_SpotLights;
}

template <>
inline std::unordered_map<EntityID, PointLight>& Registry::getStorage<PointLight>() {
    return m_PointLights;
}

} // namespace zk
