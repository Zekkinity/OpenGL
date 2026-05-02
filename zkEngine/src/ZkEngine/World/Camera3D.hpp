#pragma once

#include "glm/ext/matrix_clip_space.hpp"
#include <glm/glm.hpp>

namespace zk {

struct CameraConfig {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
};

class Camera3D {
  public:
    Camera3D(const CameraConfig& config = {}) : m_Config(config) {}
    ~Camera3D() = default;

    glm::mat4 getProjection() const {
        return glm::perspective(
            glm::radians(m_Config.fov), m_Config.aspect, m_Config.zNear, m_Config.zFar);
    }

    float getFOV() const { return m_Config.fov; }
    float getAspect() const { return m_Config.aspect; }
    float getZNear() const { return m_Config.zNear; }
    float getZFar() const { return m_Config.zFar; }

    void setFOV(float fov) { m_Config.fov = fov; }
    void setAspect(float aspect) { m_Config.aspect = aspect; }
    void setZNear(float zNear) { m_Config.zNear = zNear; }
    void setZFar(float zFar) { m_Config.zFar = zFar; }

    void onWindowResize(float width, float height) { setAspect(width / height); }

  private:
    CameraConfig m_Config;
};

} // namespace zk
