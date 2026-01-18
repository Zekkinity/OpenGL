#pragma once

#include "RenderObject.hpp"

#include "glm/ext/vector_float3.hpp"

class Camera3D;
struct Ray;

class PhysicsSystem {
   public:
    PhysicsSystem(Camera3D& camera);
    ~PhysicsSystem() = default;

    void update(RenderObject& object, float dt);
    void applyHit(const glm::vec3& hitDir, const glm::vec3& contactOffset);

    void applyGravity(RenderObject& object, float dt);

    void debug();
    void drawDebugVectors() const;

   private:
    Ray makeRayFromScreenCenter(int width, int height);
    bool raySphereIntersect(const Ray& ray, const glm::vec3& center, float radius, float& outT);

   private:
    Camera3D& m_Camera;

    bool m_MouseWasDown = false;

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
