#include "Camera3D.hpp"

#include "ZkEngine/Input/Input.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

namespace zk {

void Camera3D::onKeyPress(float deltaTime) {
    float velocity = m_Speed * deltaTime;

    m_Forward = glm::normalize(glm::vec3(m_Front.x, 0, m_Front.z));

    if (Input::IsKeyDown(Key::W))
        m_Position += m_Forward * velocity;
    if (Input::IsKeyDown(Key::S))
        m_Position -= m_Forward * velocity;
    if (Input::IsKeyDown(Key::A))
        m_Position -= m_Right * velocity;
    if (Input::IsKeyDown(Key::D))
        m_Position += m_Right * velocity;

    if (Input::IsKeyDown(Key::SPACE))
        m_Position.y += velocity;
    if (Input::IsKeyDown(Key::SHIFT))
        m_Position.y -= velocity;
}

void Camera3D::onMouseMove() {
    float xpos = Input::GetMouseX();
    float ypos = Input::GetMouseY();

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed Y
    lastX = xpos;
    lastY = ypos;

    xoffset *= m_Sensitivity;
    yoffset *= m_Sensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Clamp pitch (prevents flipping)
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera3D::onMouseScroll() {
    float yoffset = Input::GetMouseScroll();
    m_FOV -= (float)yoffset;
    m_FOV = glm::clamp(m_FOV, 1.0f, 90.0f);
    updateProjection(1280.0f, 720.0f);
}

void Camera3D::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0, 1, 0)));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));

    updateView();
}

// Updates when camera moves
void Camera3D::updateView() {
    // const float radius = 100.0f;
    // auto time = Time::getTime();
    // float camX = sin(time) * radius;
    // float camZ = cos(time) * radius;

    // m_Position = glm::vec3(camX, 0.0f, camZ);
    // m_View = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0),
    // glm::vec3(0.0, 1.0, 0.0));

    m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

// Update projection matrix
void Camera3D::updateProjection(float width, float height) {
    // m_Projection = glm::perspective(glm::radians(m_FOV), width / height, 0.1f, 5000.0f);
    m_Projection = glm::perspective(glm::radians(m_FOV), width / height, 0.1f, m_Zfar);
}

glm::mat4 Camera3D::getViewProjection() const {
    return m_Projection * m_View;
}

void Camera3D::onWindowResize(float width, float height) {
    updateProjection(width, height);
}

void Camera3D::setFOV(float fov) {
    m_FOV = fov;
}

void Camera3D::setFirstMouse(bool first) {
    firstMouse = first;
}

void Camera3D::debug() {
    ImGui::Text("Camera");

    if (ImGui::SliderFloat3("Position", &m_Position.x, -300.0f, 300.0f)) {
        updateView();
    }

    if (ImGui::SliderFloat("Yaw", &m_Yaw, -180.0f, 180.0f)) {
        updateVectors();
    }
    if (ImGui::SliderFloat("Pitch", &m_Pitch, -89.0f, 89.0f)) {
        updateVectors();
    }

    if (ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 90.0f)) {
        updateProjection(1280.0f, 720.0f);
    }

    if (ImGui::SliderFloat("Zfar", &m_Zfar, 1.0f, 10000.0f)) {
        updateProjection(1280.0f, 720.0f);
    }
}

} // namespace zk
