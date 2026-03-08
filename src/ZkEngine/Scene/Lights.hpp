#pragma once

#include <glm/glm.hpp>

namespace zk {

struct PointLight {
    glm::vec3 color{1.0f};

    float ambient = 0.8f;
    float diffuse = 0.2f;
    float specular = 1.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.03f;
};

struct DirectionalLight {
    glm::vec3 color{1.0f};

    glm::vec3 direction;

    float ambient = 0.2f;
    float diffuse = 0.8f;
    float specular = 1.0f;
};

struct SpotLight {
    glm::vec3 color{1.0f};

    glm::vec3 direction;
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));

    float ambient = 0.2f;
    float diffuse = 0.8f;
    float specular = 1.0f;

    float constant;
    float linear;
    float quadratic;
};

}
