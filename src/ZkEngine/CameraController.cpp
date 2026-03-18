#include "CameraController.hpp"

#include "ZkEngine/Camera3D.hpp"

#include "ZkEngine/Input/Input.hpp"
#include "imgui.h"

namespace zk {

CameraController::CameraController(Camera3D& camera) : m_Camera(camera) {}

void CameraController::update(float deltaTime) {
    handleKeyboard(deltaTime);

    if (m_MouseCaptured) {
        handleMouse();
        handleMouseScroll();
    }
}

void CameraController::captureMouse(bool capture) {
    m_MouseCaptured = capture;
    m_FirstMouse = true;
}

void CameraController::handleKeyboard(float deltaTime) {
    float velocity = m_Speed * deltaTime;

    glm::vec3 pos = m_Camera.getPosition();
    glm::vec3 front = m_Camera.getFront();
    glm::vec3 right = m_Camera.getRight();
    glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0, front.z));

    if (Input::IsKeyDown(Key::W))
        pos += forward * velocity;
    if (Input::IsKeyDown(Key::S))
        pos -= forward * velocity;
    if (Input::IsKeyDown(Key::A))
        pos -= right * velocity;
    if (Input::IsKeyDown(Key::D))
        pos += right * velocity;

    if (Input::IsKeyDown(Key::SPACE))
        pos.y += velocity;
    if (Input::IsKeyDown(Key::SHIFT))
        pos.y -= velocity;

    m_Camera.setPosition(pos);
}

void CameraController::handleMouse() {
    float x = Input::GetMouseX();
    float y = Input::GetMouseY();

    if (m_FirstMouse) {
        m_LastX = x;
        m_LastY = y;
        m_FirstMouse = false;
    }

    float yaw = m_Camera.getYaw() + (x - m_LastX) * m_Sensitivity;
    float pitch = m_Camera.getPitch() + (m_LastY - y) * m_Sensitivity;

    m_LastX = x;
    m_LastY = y;

    // Clamp pitch (prevents flipping)
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    m_Camera.setPitch(pitch);
    m_Camera.setYaw(yaw);
}

void CameraController::handleMouseScroll() {
    float yoffset = Input::GetMouseScroll();

    float fov = m_Camera.getFOV();
    fov -= (float)yoffset;
    fov = glm::clamp(fov, 1.0f, 90.0f);

    m_Camera.setFOV(fov);
}

void CameraController::debug() {
    ImGui::Text("Camera");

    ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 1000.0f);

    static glm::vec3 cameraPos = m_Camera.getPosition();
    if (ImGui::SliderFloat3("Position", &cameraPos.x, -300.0f, 300.0f)) {
        m_Camera.setPosition(cameraPos);
    }

    static float yaw = m_Camera.getYaw();
    if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f)) {
        m_Camera.setYaw(yaw);
    }

    static float pitch = m_Camera.getPitch();
    if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f)) {
        m_Camera.setPitch(pitch);
    }

    static float fov = m_Camera.getFOV();
    if (ImGui::SliderFloat("FOV", &fov, 1.0f, 90.0f)) {
        m_Camera.setFOV(fov);
    }

    static float zFar = m_Camera.getZFar();
    if (ImGui::SliderFloat("Zfar", &zFar, 1.0f, 10000.0f)) {
        m_Camera.setZFar(zFar);
    }
}

} // namespace zk
