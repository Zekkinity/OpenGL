#pragma once

#include <glm/glm.hpp>

namespace zk {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords; // uv
};

} // namespace zk
