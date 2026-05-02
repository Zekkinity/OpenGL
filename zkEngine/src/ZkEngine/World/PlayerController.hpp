#pragma once

#include "ZkEngine/World/Entity.hpp"

namespace zk {

class PlayerController {
  public:
    void setCamera(Entity camera);

    void update(float dt);

    void captureMouse(bool capture);

    void syncFromCamera();

  private:
    void handleKeyboard(float dt);
    void handleMouse();
    void handleMouseScroll();
    void updateRotation();

  private:
    Entity m_Camera;

    float m_Speed = 10.0f;
    float m_Sensitivity = 0.1f;

    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    bool m_MouseCaptured = false;

    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;

    glm::vec3 m_PreviousPosition = glm::vec3(0.0f);
    glm::vec3 m_PreviousVelocity = glm::vec3(0.0f);
};

} // namespace zk
