#pragma once

#include <glm/glm.hpp>

namespace zk {

struct RigidBody {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    glm::vec3 rotation;
    glm::vec3 angularVelocity;
};

} // namespace zk
