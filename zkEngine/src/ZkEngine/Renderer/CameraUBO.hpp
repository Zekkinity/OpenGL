#pragma once

#include <glm/glm.hpp>

namespace zk {

struct CameraUBO {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 viewPos;
    float nearPlane;
    float farPlane;
    float pad[2];
};

} // namespace zk
