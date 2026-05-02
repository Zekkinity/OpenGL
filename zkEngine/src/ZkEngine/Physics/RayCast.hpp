#pragma once

#include "ZkEngine/World/Camera3D.hpp"
#include <glm/glm.hpp>

namespace zk::Utils {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

// Throws a ray from the cameras position in the center of the screen
// Ray makeRayFromScreenCenter(int width, int height, EntityID camera);

bool raySphereIntersect(const Ray& ray, const glm::vec3& center, float radius, float& outT);

} // namespace zk::Utils
