#include "ZkEngine/ECS/Registry.hpp"

#include "ZkEngine/ECS/Entity.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "ZkEngine/Model.hpp"
#include <iostream>

namespace zk {

Registry::Registry() {
    // std::cout << "Registry created at " << this << "\n";
}

Registry::~Registry() {
    // std::cout << "Registry destroyed at " << this << "\n";

    clearEntities();
}

Entity Registry::createEntity() {
    uint32_t id = m_NextEntityID++;
    return Entity(id, *this);
}

void Registry::destroyEntity(EntityID id) {
    m_Transforms.erase(id);
    m_Renderers.erase(id);
    m_Models.erase(id);
    m_PointLights.erase(id);
    m_DirectionalLights.erase(id);
    m_SpotLights.erase(id);
}

void Registry::clearEntities() {
    m_Transforms.clear();
    m_Renderers.clear();
    m_Models.clear();
    m_PointLights.clear();
    m_DirectionalLights.clear();
    m_SpotLights.clear();
}

} // namespace zk
