#pragma once

#include "ZkEngine/Core/Base.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace zk {

class Camera3D;
class RenderObject;

class PhysicsSystem {
  public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;

    // void update(RenderObject& object, EntityID camera, float dt);
    void applyHit(const glm::vec3& hitDir, const glm::vec3& contactOffset);

    void applyGravity(RenderObject& object, float dt);

    void debug();
    void drawDebugVectors() const;

  private:
    static constexpr float SPHERE_MASS = 0.250f;
    static constexpr float GRAVITY = 9.81f;
    // static constexpr float GRAVITY = 0.0f;

    glm::vec3 m_Velocity = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_AngularVelocity = {
        glm::radians(0.0f), // -90°/s
        0.0f,               //
        glm::radians(0.0f)  //
    };

    glm::quat m_Orientation = glm::quat(1, 0, 0, 0);

    // Debugging vectors
    Scope<RenderObject> m_VelocityVector;
    Scope<RenderObject> m_GravityVector;
    Scope<RenderObject> m_AngularVector;
};

} // namespace zk
