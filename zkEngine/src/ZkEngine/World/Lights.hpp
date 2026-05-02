#pragma once

#include <glm/glm.hpp>

namespace zk {

struct PointLight {
    glm::vec3 color{1.0f};
    float intensity = 1.0f;

    float ambient = 0.8f;
    float diffuse = 0.2f;
    float specular = 1.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct DirectionalLight {
    glm::vec3 color{1.0f};
    float intensity = 1.0f;

    glm::vec3 direction{1.0f, 0.0f, 0.0f};

    float ambient = 0.2f;
    float diffuse = 0.8f;
    float specular = 1.0f;
};

struct SpotLight {
    glm::vec3 color{1.0f};
    float intensity = 1.0f;

    glm::vec3 direction{1.0f, 0.0f, 0.0f};
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));

    float ambient = 0.2f;
    float diffuse = 0.8f;
    float specular = 1.0f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.03f;
};

struct GPUPointLight {
    PointLight light;
    glm::vec3 position;
    uint32_t depthMap = 0;
    float farPlane = 100.0f;
};

struct GPUSpotLight {
    SpotLight light;
    glm::vec3 position;
};

struct GPUDirLight {
    DirectionalLight light;

    uint32_t depthMap = 0;
    glm::mat4 lightSpaceMatrix;
};

struct LightData {
    std::vector<GPUPointLight> pointLights;
    std::vector<GPUSpotLight> spotLights;
    std::vector<GPUDirLight> dirLights;
};

struct DirLightUBO {
    // Dir Light
    glm::vec4 direction;

    glm::vec4 intensity;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct PointLightUBO {
    glm::vec4 position;

    glm::vec4 intensity;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    glm::vec4 attenuation; // constant, linear, quadratic, farPlane
};

struct SpotLightUBO {
    glm::vec4 position;
    glm::vec4 direction;

    float cutOff;
    float outerCutOff;
    float pad[2];

    glm::vec4 intensity;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    glm::vec4 attenuation; // constant, linear, quadratic, farPlane
};

constexpr int MAX_POINT_LIGHTS = 16;

// NOTE: Cant pass textures like depthMap in a UBO
struct LightsUBO {
    int pointLightCount = 0;
    int dirLightCount = 0;
    int spotLightCount = 0;
    int _padding = 0;

    PointLightUBO pointLights[MAX_POINT_LIGHTS];
    DirLightUBO dirLights[4];
    SpotLightUBO spotLights[4];
};

} // namespace zk
