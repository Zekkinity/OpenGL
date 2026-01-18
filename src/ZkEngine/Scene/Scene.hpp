#pragma once

#include <vector>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/ModelObject.hpp"
#include "ZkEngine/PhysicsSystem.hpp"
#include "ZkEngine/RenderObject.hpp"

#include <glm/gtc/quaternion.hpp>

class Renderer;

struct Particle {
    RenderObject* object;
    glm::vec3 velocity = glm::vec3(0.0f);
    int charge;
};

class Scene {
   public:
    Scene(bool floor);
    ~Scene();

    void init();
    void update(float dt);

    void submit(Renderer& renderer) const;

    void addRenderObject(std::unique_ptr<RenderObject> renderObject);

    // void addRenderObject(Mesh* mesh, Material* material);
    // void addRandomObject(Mesh* mesh, Material* material);

    Camera3D& getCamera() { return m_Camera; }

    void floorScene();

    void debug();

    void manageElectrons(float dt);

   private:
    std::vector<std::unique_ptr<RenderObject>> m_Objects;

    std::unique_ptr<ModelObject> m_ModelObject;

    std::shared_ptr<Shader> toonShader;

    Camera3D m_Camera;

    PhysicsSystem m_PhysicsSystem;

    glm::vec3 p1 = glm::vec3(0.0f);
    glm::vec3 p2 = glm::vec3(0.0f);

    float r = 0.0f;
    float angle = 0.0f;

    RenderObject* currentObject = nullptr;

    std::unique_ptr<RenderObject> sphereObject;
    std::unique_ptr<RenderObject> negativeObject;

    Particle m_PositiveParticle;
    Particle m_NegativeParticle;

    constexpr static double COULOMB_CONSTANT = 8.99e9;
    constexpr static double ELECTRON_CHARGE = 1.60e-19;
    constexpr static float ELECTRON_MASS = 9.11e-31;

    // static constexpr float K = 1.0f;
    static constexpr float K = 300.0f;
    static constexpr float Q1 = 1.0f;
    static constexpr float Q2 = -1.0f;
    static constexpr float MASS = 1.0f;
};
