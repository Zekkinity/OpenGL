#pragma once

#include <glm/glm.hpp>

namespace zk {

struct CameraState {
    glm::vec3 position;
    float yaw;
    float pitch;
};

struct CameraConfig {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
};

class Camera3D {
  public:
    Camera3D();
    Camera3D(const CameraConfig& config);

    ~Camera3D() = default;

    CameraState getState() const { return {m_Position, m_Yaw, m_Pitch}; }
    void setState(const CameraState& state) {
        m_Position = state.position;
        m_Yaw = state.yaw;
        m_Pitch = state.pitch;
        recalculateView();
    }

    const glm::mat4& getView();
    const glm::mat4& getProjection();

    glm::vec3 getPosition() const { return m_Position; }
    glm::vec3 getFront() const { return m_Front; }
    glm::vec3 getRight() const { return m_Right; }
    glm::vec3 getUp() const { return m_Up; }
    float getYaw() const { return m_Yaw; }
    float getPitch() const { return m_Pitch; }

    float getFOV() const { return m_Config.fov; }
    float getAspect() const { return m_Config.aspect; }
    float getZNear() const { return m_Config.zNear; }
    float getZFar() const { return m_Config.zFar; }

    void setPosition(glm::vec3 pos);
    void setYaw(float yaw);
    void setPitch(float pitch);

    void setFOV(float fov);
    void setAspect(float aspect);
    void setZNear(float zNear);
    void setZFar(float zFar);

    void onWindowResize(float width, float height);

    void debug();

  private:
    void recalculate();
    void recalculateProjection();
    void recalculateView();

  private:
    glm::mat4 m_View = glm::mat4(1.0f);
    glm::mat4 m_Projection = glm::mat4(1.0f);

    glm::vec3 m_Position = glm::vec3(0.0f, 1.0f, 20.0f);

    float m_Yaw = -90.0f; // left/right
    float m_Pitch = 0.0f; // up/down

    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

    CameraConfig m_Config;

    bool m_ViewDirty = true;
    bool m_ProjectionDirty = true;
};

} // namespace zk
