#pragma once

#include "ZkEngine/ECS/Registry.hpp"
#include "ZkEngine/Model.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "glm/ext/quaternion_float.hpp"

namespace zk {

class Scene;

using Vec3 = glm::vec3;
using Quat = glm::quat;

// Entity is a class that lets the sandbox create entities
class Entity {
  public:
    Entity() = delete;

    void addRenderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
        addComponent<RenderObject>(std::move(mesh), std::move(material));
    }

    void addModel(const std::filesystem::path& path, std::shared_ptr<Shader> shader) {
        addComponent<Model>(path, std::move(shader));
    }

    PointLight& addPointLight(PointLight light = {}) { return addComponent<PointLight>(light); }
    SpotLight& addSpotLight(SpotLight light = {}) { return addComponent<SpotLight>(light); }
    DirectionalLight& addDirectionalLight(DirectionalLight light = {}) {
        return addComponent<DirectionalLight>(light);
    }

    PointLight& getPointLight() { return getComponent<PointLight>(); }
    SpotLight& getSpotLight() { return getComponent<SpotLight>(); }
    DirectionalLight& getDirectionalLight() { return getComponent<DirectionalLight>(); }

    void setPosition(const Vec3& position);
    void setScale(const Vec3& scale);

    void setRotation(const Vec3& rotation);
    void setRotation(const Quat& rotation);

    Vec3 getPosition() const;

  private:
    Entity(EntityID id, Registry& registry) : m_ID(id), m_Registry(registry) {
        registry.addComponent<Transform3D>(id);
    }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        // ZK_CORE_INFO("Entity using registry {}", m_Registry);
        return m_Registry.addComponent<T>(m_ID, std::forward<Args>(args)...);
    }

    template <typename T>
    T& getComponent() {
        return m_Registry.getComponent<T>(m_ID);
    }

    // EntityID getID() const { return m_ID; }

  private:
    EntityID m_ID;
    Registry& m_Registry;

    friend class Registry;
    friend class ModelSystem;
};

} // namespace zk
