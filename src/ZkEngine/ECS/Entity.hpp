#pragma once

#include "ZkEngine/ECS/Registry.hpp"

namespace zk {

class Scene;

// Entity is a class that lets the sandbox create entities
class Entity {
  public:
    // NOTE: This is bad, entity should not have a default constructor
    Entity() = default;

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        // ZK_CORE_INFO("Entity using registry {}", m_Registry);
        return m_Registry->addComponent<T>(m_ID, std::forward<Args>(args)...);
    }

    template <typename T>
    T& getComponent() {
        return m_Registry->getComponent<T>(m_ID);
    }

    EntityID getID() const { return m_ID; }

  private:
    Entity(EntityID id, Registry* registry) : m_ID(id), m_Registry(registry) {}

    EntityID m_ID;
    Registry* m_Registry = nullptr;

    friend class Registry;
};

} // namespace zk
