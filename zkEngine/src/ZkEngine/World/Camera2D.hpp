#pragma once

#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

struct Camera2D {
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float zoom = 1.0f;
    float angle = 0.0f;
    glm::mat4 view = glm::mat4(1.0f);

    void update() {
        view = glm::mat4(1.0f);

        view = glm::rotate(view, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, -position);
        view = glm::scale(view, glm::vec3(zoom, zoom, 1));
    }
};
