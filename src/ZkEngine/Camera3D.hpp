#pragma once

#include <glm/glm.hpp>

struct CameraState {
    glm::vec3 position;
    float yaw;
    float pitch;
};

class Camera3D {
   public:
    CameraState getState() const { return {position, yaw, pitch}; }
    void setState(const CameraState& state) {
        position = state.position;
        yaw = state.yaw;
        pitch = state.pitch;
        updateVectors();
        updateView();
    }

    glm::vec3 getPosition() const { return position; }
    void setPosition(glm::vec3 pos) {
        position = pos;
        updateView();
    }

    void onKeyPress(float deltaTime);
    void onMouseMove();
    void updateMovement(float deltaTime);

    void updateVectors();
    void updateView();

    void onWindowResize(float width, float height);

    glm::mat4 getViewProjection() const;

    void setFirstMouse(bool first);

    void debug();

   private:
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;  // left/right
    float pitch = 0.0f;  // up/down

    float sensitivity = 0.1f;
    float speed = 60.0f;

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::mat4 view = glm::mat4(1.0f);

    float fov = 45.0f;

    // Mouse
    bool firstMouse = true;
    float lastX = 960.0f;
    float lastY = 540.0f;
};
