#pragma once

#include "ZkEngine/World/World.hpp"
#include "ZkEngine/World/Components.hpp"

#include "ZkEngine/World/Camera3D.hpp"
#include "ZkEngine/Core/Base.hpp"

namespace zk {

using Vec3 = glm::vec3;
using Quat = glm::quat;

// Entity is a class that lets the sandbox create entities
class Entity {
  public:
    Entity() = default;

    operator bool() const { return isValid(); }

    void addRenderable(Ref<Mesh> mesh, Ref<Material> material) {
        addComponent<RenderObject>(std::move(mesh), std::move(material));
    }

    void addModel(Ref<Model> model, Ref<Shader> shader) {
        addComponent<ModelComponent>(std::move(model), std::move(shader));
    }

    Camera3D& addCamera(const Camera3D& camera = {}) { return addComponent<Camera3D>(camera); }

    PointLight& addPointLight(PointLight light = {}) { return addComponent<PointLight>(light); }
    SpotLight& addSpotLight(SpotLight light = {}) { return addComponent<SpotLight>(light); }
    DirectionalLight& addDirectionalLight(DirectionalLight light = {}) {
        return addComponent<DirectionalLight>(light);
    }

    PointLight& getPointLight() { return getComponent<PointLight>(); }
    SpotLight& getSpotLight() { return getComponent<SpotLight>(); }
    DirectionalLight& getDirectionalLight() { return getComponent<DirectionalLight>(); }

    Camera3D& getCamera() { return getComponent<Camera3D>(); }

    RigidBody& addRigidBody() { return addComponent<RigidBody>(); }

    void setPosition(const Vec3& position);
    void setScale(const Vec3& scale);
    void setScale(float scale) { setScale({scale, scale, scale}); }
    void setScale(float x, float y, float z) { setScale({x, y, z}); }

    void setRotation(const Vec3& rotation);
    void setRotation(const Quat& rotation);

    Vec3 getPosition() const;

    void setColor(const glm::vec4& color);

    // EntityID getID() const { return m_ID; }

    template <typename T>
    T& getComponent() const {
        ZK_CORE_ASSERT(
            isValid(), "Entity is not initialized, did you forget to call createEntity()?");
        return m_World->m_Registry.get<T>(m_EntityHandle);
    }

  private:
    Entity(entt::entity entity, World* scene) : m_EntityHandle(entity), m_World(scene) {}

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(!std::is_empty_v<T>, "Use addTag() for empty components");

        ZK_CORE_ASSERT(!hasComponent<T>(), "Entity already has a component");

        return m_World->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
    void addTag() {
        static_assert(std::is_empty_v<T>, "Use addComponent() for non-empty components");

        ZK_CORE_ASSERT(!hasComponent<T>(), "Entity already has a component");

        m_World->m_Registry.emplace<T>(m_EntityHandle);
    }

    template <typename T>
    bool hasComponent() const {
        return m_World->m_Registry.all_of<T>(m_EntityHandle);
    }

    bool isValid() const { return m_World && m_World->m_Registry.valid(m_EntityHandle); }

  private:
    entt::entity m_EntityHandle = entt::null;
    World* m_World = nullptr;

    friend class World;
    friend class ModelSystem;
};

} // namespace zk
