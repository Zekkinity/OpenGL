#pragma once

#include <glm/glm.hpp>

namespace zk {

struct CameraState {
    glm::vec3 position;
    float yaw;
    float pitch;
};

class Camera3D {
  public:
    CameraState getState() const { return {m_Position, m_Yaw, m_Pitch}; }
    void setState(const CameraState& state) {
        m_Position = state.position;
        m_Yaw = state.yaw;
        m_Pitch = state.pitch;
        updateVectors();
    }

    glm::vec3 getPosition() const { return m_Position; }
    void setPosition(glm::vec3 pos) {
        m_Position = pos;
        updateView();
    }

    glm::vec3 getFront() const { return m_Front; }

    void setFOV(float fov);

    glm::mat4 getViewProjection() const;

    void onKeyPress(float deltaTime);
    void onMouseMove();
    void onMouseScroll();

    void onWindowResize(float width, float height);
    void setFirstMouse(bool first);

    void debug();

    void updateView();

  private:
    void updateProjection(float width, float height);

    void updateVectors();

  private:
    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Projection = glm::mat4(1.0f);

    glm::vec3 m_Position = glm::vec3(0.0f, 1.0f, 20.0f);

    float m_Yaw = -90.0f; // left/right
    float m_Pitch = 0.0f; // up/down

    float m_Sensitivity = 0.1f;
    float m_Speed = 60.0f;

    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 m_Forward;

    float m_FOV = 45.0f;

    // Mouse
    bool firstMouse = true;
    float lastX = 960.0f;
    float lastY = 540.0f;
};

} // namespace zk
