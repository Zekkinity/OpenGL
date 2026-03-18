#include "Camera3D.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

namespace zk {

Camera3D::Camera3D() {
    recalculate();
}

Camera3D::Camera3D(const CameraConfig& config) : m_Config(config) {
    recalculate();
}

void Camera3D::recalculate() {
    recalculateProjection();
    recalculateView();
}

const glm::mat4& Camera3D::getView() {
    if (m_ViewDirty) {
        recalculateView();
    }

    return m_View;
}

const glm::mat4& Camera3D::getProjection() {
    if (m_ProjectionDirty) {
        recalculateProjection();
    }

    return m_Projection;
}

// Updates when camera moves
void Camera3D::recalculateView() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0, 1, 0)));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));

    m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

    m_ViewDirty = false;
}

// Update projection matrix
void Camera3D::recalculateProjection() {
    // m_Projection = glm::perspective(glm::radians(m_FOV), width / height, 0.1f, 5000.0f);
    m_Projection = glm::perspective(
        glm::radians(m_Config.fov), m_Config.aspect, m_Config.zNear, m_Config.zFar);
    m_ProjectionDirty = false;
}

void Camera3D::onWindowResize(float width, float height) {
    setAspect(width / height);
}

void Camera3D::setPosition(glm::vec3 pos) {
    m_Position = pos;
    m_ViewDirty = true;
}

void Camera3D::setPitch(float pitch) {
    m_Pitch = pitch;
    m_ViewDirty = true;
}

void Camera3D::setYaw(float yaw) {
    m_Yaw = yaw;
    m_ViewDirty = true;
}

void Camera3D::setFOV(float fov) {
    m_Config.fov = fov;
    m_ProjectionDirty = true;
}

void Camera3D::setAspect(float aspect) {
    m_Config.aspect = aspect;
    m_ProjectionDirty = true;
}

void Camera3D::setZNear(float zNear) {
    m_Config.zNear = zNear;
    m_ProjectionDirty = true;
}

void Camera3D::setZFar(float zFar) {
    m_Config.zFar = zFar;
    m_ProjectionDirty = true;
}

void Camera3D::debug() {
    ImGui::Text("Camera");

    // ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 1000.0f);

    static glm::vec3 cameraPos = getPosition();
    if (ImGui::SliderFloat3("Position", &cameraPos.x, -300.0f, 300.0f)) {
        setPosition(cameraPos);
    }

    static float yaw = getYaw();
    if (ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f)) {
        setYaw(yaw);
    }

    static float pitch = getPitch();
    if (ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f)) {
        setPitch(pitch);
    }

    static float fov = getFOV();
    if (ImGui::SliderFloat("FOV", &fov, 1.0f, 90.0f)) {
        setFOV(fov);
    }

    static float zFar = getZFar();
    if (ImGui::SliderFloat("Zfar", &zFar, 1.0f, 10000.0f)) {
        setZFar(zFar);
    }

}

} // namespace zk
