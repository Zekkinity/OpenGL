#include "zkpch.hpp"

#include "ZkEngine/World/PlayerController.hpp"

#include "ZkEngine/Input/Input.hpp"
#include "ZkEngine/World/Entity.hpp"

namespace zk {

void PlayerController::setCamera(Entity camera) {
    m_Camera = camera;
    syncFromCamera();
}

void PlayerController::update(float dt) {
    if (!m_MouseCaptured || !m_Camera)
        return;

    handleKeyboard(dt);
    handleMouse();
    handleMouseScroll();
}

void PlayerController::captureMouse(bool capture) {
    m_MouseCaptured = capture;
    m_FirstMouse = true;
}

void PlayerController::syncFromCamera() {
    auto& transform = m_Camera.getComponent<Transform3D>();

    glm::vec3 forward = transform.getForward();

    m_Yaw = glm::degrees(atan2(forward.z, forward.x));
    m_Pitch = glm::degrees(asin(-forward.y));
}

void PlayerController::handleKeyboard(float dt) {
    auto& transform = m_Camera.getComponent<Transform3D>();

    float speed = m_Speed;
    if (Input::IsKeyDown(Key::CONTROL)) {
        speed *= 2.5f;
    }

    float velocity = speed * dt;

    glm::vec3 forward = transform.getForward();
    glm::vec3 flatForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    if (Input::IsKeyDown(Key::W))
        transform.position += flatForward * velocity;

    if (Input::IsKeyDown(Key::S))
        transform.position -= flatForward * velocity;

    if (Input::IsKeyDown(Key::A))
        transform.position -= transform.getRight() * velocity;

    if (Input::IsKeyDown(Key::D))
        transform.position += transform.getRight() * velocity;

    if (Input::IsKeyDown(Key::SPACE))
        transform.position.y += velocity;
    if (Input::IsKeyDown(Key::SHIFT))
        transform.position.y -= velocity;

    glm::vec3 dpos = transform.position - m_PreviousPosition;
    glm::vec3 velo = dpos / dt;
    // ZK_CORE_DEBUG("Velocity: [{},{},{}] m/s", velo.x, velo.y, velo.z);

    glm::vec3 dAcc = velo - m_PreviousVelocity;
    glm::vec3 acc = dAcc / dt;
    // ZK_CORE_DEBUG("Acceleration: [{},{},{}] m/s^2", acc.x, acc.y, acc.z);

    m_PreviousPosition = transform.position;
    m_PreviousVelocity = velo;
}

void PlayerController::handleMouse() {
    float x = Input::GetMouseX();
    float y = Input::GetMouseY();

    if (m_FirstMouse) {
        m_LastX = x;
        m_LastY = y;
        m_FirstMouse = false;
    }

    float dx = (x - m_LastX) * m_Sensitivity;
    float dy = (m_LastY - y) * m_Sensitivity;

    m_LastX = x;
    m_LastY = y;

    // update angles
    m_Yaw += dx;
    m_Pitch += dy;

    // clamp pitch
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

    updateRotation();
}

void PlayerController::handleMouseScroll() {
    auto& camera = m_Camera.getComponent<Camera3D>();

    float yoffset = Input::GetMouseScroll();

    float fov = camera.getFOV();
    fov -= (float)yoffset;
    fov = glm::clamp(fov, 1.0f, 90.0f);

    camera.setFOV(fov);
}

void PlayerController::updateRotation() {
    auto& transform = m_Camera.getComponent<Transform3D>();

    glm::vec3 forward;
    forward.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    forward.y = sin(glm::radians(m_Pitch));
    forward.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    forward = glm::normalize(forward);

    transform.lookAt(forward);
}

} // namespace zk
