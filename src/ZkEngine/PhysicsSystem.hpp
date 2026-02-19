#pragma once

#include "RenderObject.hpp"

#include "glm/ext/vector_float3.hpp"

namespace zk {

class Camera3D;

class PhysicsSystem {
   public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;

    void update(RenderObject& object, Camera3D& camera, float dt);
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
        glm::radians(0.0f),  // -90°/s
        0.0f,                //
        glm::radians(0.0f)   //
    };

    glm::quat m_Orientation = glm::quat(1, 0, 0, 0);

    // Debugging vectors
    std::unique_ptr<RenderObject> m_VelocityVector;
    std::unique_ptr<RenderObject> m_GravityVector;
    std::unique_ptr<RenderObject> m_AngularVector;
};

}
