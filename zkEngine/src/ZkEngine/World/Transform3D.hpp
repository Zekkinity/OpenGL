#pragma once

#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>

namespace zk {

struct Transform3D {
  private:
    glm::quat m_Rotation = glm::quat(1, 0, 0, 0);

  public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    glm::mat4 getMatrix() const {
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);

        model *= glm::mat4_cast(m_Rotation);

        model = glm::scale(model, scale);

        return model;
    }

    glm::mat4 getView() const {
        glm::vec3 forward = getForward();
        glm::vec3 up = getUp();

        return glm::lookAt(position, position + forward, up);
    }

    void setRotation(const glm::vec3& euler) { m_Rotation = glm::quat(glm::radians(euler)); }

    glm::vec3 getRotation() const { return glm::degrees(glm::eulerAngles(m_Rotation)); }

    void lookAt(const glm::vec3& target, const glm::vec3& up = {0, 1, 0}) {
        m_Rotation = glm::quatLookAt(glm::normalize(target), up);
    }

    glm::vec3 getForward() const { return m_Rotation * glm::vec3(0, 0, -1); }

    glm::vec3 getRight() const { return m_Rotation * glm::vec3(1, 0, 0); }

    glm::vec3 getUp() const { return m_Rotation * glm::vec3(0, 1, 0); }
};

} // namespace zk
