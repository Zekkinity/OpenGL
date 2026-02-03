#include "PhysicsSystem.hpp"

#include <GLFW/glfw3.h>
#include "Camera3D.hpp"
#include "Input/Input.hpp"
#include "imgui.h"
#include "Utils/Time.hpp"

namespace zk {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

// Throws a ray from the cameras position in the center of the screen
Ray PhysicsSystem::makeRayFromScreenCenter(int width, int height, Camera3D& camera) {
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

bool PhysicsSystem::raySphereIntersect(
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

// void PhysicsSystem::init() {
// // DEBUG: Init vectors
// m_VelocityVector = std::make_unique<RenderObject>(velocityMesh, normalMaterial);
// m_VelocityVector->setColor({1.0f, 0.0f, 0.0f, 1.0f});
//
// m_GravityVector = std::make_unique<RenderObject>(gravityMesh, normalMaterial);
// m_GravityVector->setColor({0.0f, 1.0f, 0.0f, 1.0f});
//
// m_AngularVector = std::make_unique<RenderObject>(angularMesh, normalMaterial);
// m_AngularVector->setColor({0.0f, 1.0f, 0.0f, 1.0f});
// }

void PhysicsSystem::update(RenderObject& object, Camera3D& camera, float dt) {
    // applyGravity(object, dt);

    if (Input::IsMousePressed(MouseButton::Left)) {
        Ray ray = makeRayFromScreenCenter(1920, 1080, camera);

        float t;
        if (raySphereIntersect(ray, object.transform.position, 2.0f, t)) {
            glm::vec3 hitPoint = ray.origin + ray.direction * t;
            glm::vec3 contactOffset = hitPoint - object.transform.position;

            applyHit(ray.direction, contactOffset);
        }
    }

    float speed = glm::length(m_Velocity);
    glm::vec3 acceleration = glm::vec3(0.0f);

    // Magnus coeff will make the curve more exagerated
    float MAGNUS_COEFF = 0.00015f; // try 0.006 - 0.0025
    if (speed > 0.01f) {
        glm::vec3 effectiveSpin = m_AngularVelocity * 0.5f;

        glm::vec3 magnusAcc = MAGNUS_COEFF * speed * glm::cross(effectiveSpin, m_Velocity);
        acceleration += magnusAcc;
    }

    // Adjust this value to change how fast the object slows down
    float DRAG_COEFF = 0.0002f; // try 0.03 – 0.1
    glm::vec3 dragAcc = -DRAG_COEFF * m_Velocity * glm::length(m_Velocity);
    acceleration += dragAcc;

    m_Velocity += acceleration * dt;

    // Y: 0
    if (object.transform.position.y < 0.0f) {
        const glm::vec3 floorNormal = {0.0f, 1.0f, 0.0f};
        const float floorHeight = 0.0f;

        object.transform.position.y = 0.0f;

        float vn = glm::dot(m_Velocity, floorNormal);
        glm::vec3 vN = vn * floorNormal;
        glm::vec3 vT = m_Velocity - vN;
        // std::println("vT: {} {} {}", vT.x, vT.y, vT.z);

        float restitution = 0.6f;
        float friction = 0.4f;

        if (vn < 0.0f) {
            vN = -vN * restitution;
        }

        vT *= 1.0f - friction;
        m_Velocity = vN + vT;

        glm::vec3 r = -floorNormal * 2.5f;

        glm::vec3 frictionImpulse = -vT * 0.2f;
        m_AngularVelocity += glm::cross(r, frictionImpulse);

        // auto theta = glm::acos((velocity * floorNormal / glm::length(velocity) *
        // glm::length(floorNormal))); float thetaY = glm::degrees(theta.y); std::cout <<
        // "theta.y: " << thetaY << std::endl; velocity.y = velocity.y * -0.7f;
    }
    object.transform.position += m_Velocity * dt;

    float angle = glm::length(m_AngularVelocity) * dt;
    if (angle > 0.0f) {
        glm::vec3 axis = glm::normalize(m_AngularVelocity);
        glm::quat dq = glm::angleAxis(angle, axis);
        m_Orientation = glm::normalize(m_Orientation * dq);
    }
    object.transform.rotationQuat = m_Orientation;

    float angularDamping = 0.2f; // per second-ish
    m_AngularVelocity *= exp(-angularDamping * dt);

    // // DEBUG: Draw velocity vector
    // // m_VelocityVector->mesh->updateVertices(object.transform.position,
    // //                                        object.transform.position + (velocity * 0.4f));
    // // m_GravityVector->mesh->updateVertices(
    // //     object.transform.position, object.transform.position + glm::vec3(0.0f, -GRAVITY,
    // 0.0f));
    // // m_AngularVector->mesh->updateVertices(object.transform.position,
    // //                                       object.transform.position + angularVelocity);
}

void PhysicsSystem::applyHit(const glm::vec3& hitDir, const glm::vec3& contactOffset) {
    float radius = 2.0f;
    float dt = Time::getDeltaTime();

    glm::vec3 limit = glm::vec3(0.5, 0.5, 0.5);
    glm::vec3 finalOffset = contactOffset;

    if ((contactOffset.y < limit.y && contactOffset.y > -limit.y) &&
        (contactOffset.x < limit.x && contactOffset.x > -limit.x)) {
        finalOffset = glm::vec3(0, 0, 0);
    }

    // glm::vec3 hitDir = glm::normalize(glm::vec3(0, 0, -1));  // forward

    // +Y = Hit above center (topspin)
    // -Y = Hit below center (backspin)
    // glm::vec3 contactOffset = glm::vec3(-radius, radius, 0);
    // glm::vec3 contactOffset = glm::vec3(-1.0f, 0.2f, 0) * radius;

    // float hitStrength = 60'000.0f;
    float hitStrength = 40'200.0f;
    float spinRatio = 0.15f;

    // Impulse
    glm::vec3 impulse = glm::normalize(hitDir) * hitStrength * dt;
    glm::vec3 angularImpulse = glm::cross(finalOffset, impulse) * spinRatio;

    m_AngularVelocity += angularImpulse;
    // Linear impulse
    m_Velocity += impulse;
}

void PhysicsSystem::applyGravity(RenderObject& object, float dt) {
    glm::vec3 gravityAcc = {0.0f, -GRAVITY, 0.0f};
    m_Velocity += gravityAcc * dt;
    object.transform.position += m_Velocity;
}

void PhysicsSystem::debug() {
    if (ImGui::Button("Apply Force")) {
        float radius = 2.0f;
        applyHit(glm::vec3(0, 0, -1), glm::vec3(0, radius, 0));
    }

    ImGui::Text("Velocity: %.2f %.2f %.2f", m_Velocity.x, m_Velocity.y, m_Velocity.z);
    ImGui::Text(
        "Angular Vel: %.2f %.2f %.2f",
        m_AngularVelocity.x,
        m_AngularVelocity.y,
        m_AngularVelocity.z);
}

void PhysicsSystem::drawDebugVectors() const {
    // DEBUG: Draw vectors
    // m_VelocityVector->draw(m_Camera.getViewProjection());
    // m_GravityVector->draw(m_Camera.getViewProjection());
    // m_AngularVector->draw(m_Camera.getViewProjection());
}

} // namespace zk
