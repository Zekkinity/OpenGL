#pragma once

#include "ZkEngine/World/Camera3D.hpp"
#include <glm/glm.hpp>

#include "ZkEngine/World/Entity.hpp"

namespace zk::Utils {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

// Throws a ray from the cameras position in the center of the screen
Ray makeRayFromScreenCenter(int width, int height, Entity camera);

bool raySphereIntersect(const Ray& ray, const glm::vec3& center, float radius, float& outT);

bool rayAABBIntersect(const Ray& ray, const AABB& aabb, float& t);

} // namespace zk::Utils
