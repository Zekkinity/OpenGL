#include "Camera3D.hpp"

#include "ZkEngine/Input/Input.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

void Camera3D::updateMovement(float deltaTime) {
    onKeyPress(deltaTime);
    onMouseMove();
}

void Camera3D::onKeyPress(float deltaTime) {
    float velocity = speed * deltaTime;

    glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0, front.z));

    if (Input::IsKeyDown(Key::W))
        position += forward * velocity;
    if (Input::IsKeyDown(Key::S))
        position -= forward * velocity;
    if (Input::IsKeyDown(Key::A))
        position -= right * velocity;
    if (Input::IsKeyDown(Key::D))
        position += right * velocity;

    if (Input::IsKeyDown(Key::SPACE))
        position.y += velocity;
    if (Input::IsKeyDown(Key::SHIFT))
        position.y -= velocity;
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
    float yoffset = lastY - ypos;  // reversed Y

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch (prevents flipping)
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera3D::updateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, front));
}

// Updates when camera moves
void Camera3D::updateView() {
    view = glm::lookAt(position, position + front, up);
}

// Update projection matrix
void Camera3D::onWindowResize(float width, float height) {
    projection = glm::perspective(glm::radians(fov), width / height, 0.1f, 5000.0f);
}

glm::mat4 Camera3D::getViewProjection() const {
    return projection * view;
}

void Camera3D::setFirstMouse(bool first) {
    firstMouse = first;
}

void Camera3D::debug() {
    ImGui::Text("Camera");

    ImGui::SliderFloat3("Position", &position.x, -300.0f, 300.0f);
    ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f);
}
