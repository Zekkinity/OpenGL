#pragma once

#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

struct Transform2D {
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 scale = glm::vec2(1.0f, 1.0f);
    float rotation = 0.0f;

    glm::mat4 getMatrix() const {
        glm::mat4 model(1.0f);

        // Correct order is translation -> rotation -> scale
        model = glm::translate(model, {position.x, position.y, 0.0f});

        // If you rotate after translation, it will rotate around the center of the object
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, {scale.x, scale.y, 1.0f});

        return model;
    }
};

