#pragma once

#include <glm/glm.hpp>

namespace zk {

struct CameraUBO {
    glm::mat4 viewProj;
    glm::vec4 viewPos;
};

} // namespace zk
