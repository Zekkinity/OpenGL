#pragma once

#include "ZkEngine/Scene/Scene.hpp"

namespace zk {

class Scene;

class Entity {
  public:
    Entity() = default;
    Entity(EntityID id, Scene* scene) : m_ID(id), m_Scene(scene) {}

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        return m_Scene->addComponent<T>(m_ID, std::forward<Args>(args)...);
    }

    template <typename T>
    T& getComponent() {
        return m_Scene->getComponent<T>(m_ID);
    }

    EntityID getID() const { return m_ID; }

  private:
    EntityID m_ID;
    Scene* m_Scene = nullptr;
};

} // namespace zk
