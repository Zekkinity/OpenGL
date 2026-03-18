#pragma once

namespace zk {

class Camera3D;

class CameraController {
  public:
    CameraController() = delete;
    CameraController(Camera3D& camera);

    ~CameraController() = default;

    void update(float deltaTime);
    void captureMouse(bool capture);

    void debug();

  private:
    void handleKeyboard(float deltaTime);
    void handleMouse();
    void handleMouseScroll();

  private:
    Camera3D& m_Camera;

    float m_Speed = 10.0f;
    float m_Sensitivity = 0.1f;

    bool m_MouseCaptured = false;

    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
};

} // namespace zk
