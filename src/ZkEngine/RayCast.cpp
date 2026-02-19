#include "ZkEngine/RayCast.hpp"

namespace zk::Utils {

// Throws a ray from the cameras position in the center of the screen
Ray makeRayFromScreenCenter(int width, int height, Camera3D& camera) {
    glm::vec2 screenPos(width * 0.5f, height * 0.5f);

    glm::vec4 ndc;
    ndc.x = (2.0f * screenPos.x) / width - 1.0f;
    ndc.y = 1.0f - (2.0f * screenPos.y) / height;
    ndc.z = -1.0f;
    ndc.w = 1.0f;

    glm::mat4 invVP = glm::inverse(camera.getViewProjection());
    glm::vec4 worldPos = invVP * ndc;
    worldPos /= worldPos.w;

    glm::vec3 origin = camera.getPosition();
    glm::vec3 direction = glm::normalize(glm::vec3(worldPos) - origin);

    return {origin, direction};
}

bool raySphereIntersect(
    const Ray& ray, const glm::vec3& center, float radius, float& outT) {
    glm::vec3 oc = ray.origin - center;

    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0f)
        return false;

    outT = (-b - sqrt(discriminant)) / (2.0f * a);
    return outT >= 0.0f;
}

} // namespace zk::Utils
