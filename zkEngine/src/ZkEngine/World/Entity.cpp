#include "zkpch.hpp"
#include "ZkEngine/World/Entity.hpp"

namespace zk {

void Entity::setPosition(const Vec3& position) {
    Transform3D& transform = getComponent<Transform3D>();
    transform.position = position;
}

void Entity::setScale(const Vec3& scale) {
    Transform3D& transform = getComponent<Transform3D>();
    transform.scale = scale;
}

void Entity::setRotation(const Vec3& rotation) {
    Transform3D& transform = getComponent<Transform3D>();
    transform.setRotation(rotation);
}

Vec3 Entity::getPosition() const {
    Transform3D& transform = getComponent<Transform3D>();
    return transform.position;
}

void Entity::setColor(const glm::vec4& color) {
    RenderObject& renderObject = getComponent<RenderObject>();
    renderObject.material->setColor(color);
}

} // namespace zk
