#pragma once

#include <glm/glm.hpp>

namespace zk {

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texCoords{}; // uv
    
    glm::vec3 tangent{};
    glm::vec3 bitangent{};
};

} // namespace zk
