#pragma once

#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>

namespace zk {

struct Transform3D {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::quat rotationQuat = glm::quat(1, 0, 0, 0);

    glm::mat4 getMatrix() const {
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);

        // model *= glm::mat4_cast(rotationQuat);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

        model = glm::scale(model, scale);

        return model;
    }
};

} // namespace zk
