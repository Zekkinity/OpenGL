#include "ZkEngine/ECS/Entity.hpp"

namespace zk {

void Entity::setPosition(const Vec3& position) {
    Transform3D& transform = m_Registry.getComponent<Transform3D>(m_ID);
    transform.position = position;
}

void Entity::setScale(const Vec3& scale) {
    Transform3D& transform = m_Registry.getComponent<Transform3D>(m_ID);
    transform.scale = scale;
}

void Entity::setRotation(const Vec3& rotation) {
    Transform3D& transform = m_Registry.getComponent<Transform3D>(m_ID);
    transform.rotation = rotation;
}

void Entity::setRotation(const Quat& rotation) {
    Transform3D& transform = m_Registry.getComponent<Transform3D>(m_ID);
    transform.rotationQuat = rotation;
}

Vec3 Entity::getPosition() const {
    const Transform3D& transform = m_Registry.getComponent<Transform3D>(m_ID);
    return transform.position;
}

} // namespace zk
