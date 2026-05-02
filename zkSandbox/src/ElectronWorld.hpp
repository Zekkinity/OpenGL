#include "ZkEngine/World/World.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/World/Entity.hpp"
#include "ZkEngine/Utils/MeshFactory.hpp"

struct Particle {
    zk::Entity entity;
    zk::RenderObject& object;
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 force = glm::vec3(0.0f);
    float charge = 0.0f;
};

class ElectronWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    // void submit(zk::RendererBackend& renderer) const override;

    void debug() override;

  private:
    void manageElectrons(float dt);
    void addParticle(const glm::vec3& position, const glm::vec3& velocity, float charge);
    void calculateElectricField();

    glm::vec3 getElectricFieldAtPoint(const glm::vec3& position);

    std::vector<glm::vec3> traceFieldLine(
        glm::vec3 start,
        float direction,
        float minStep,
        float maxStep,
        float stepScale,
        float stopField,
        float stopRadius,
        float worldLimit,
        int maxSteps);

  private:
    std::shared_ptr<zk::Mesh> m_SphereMesh;
    std::shared_ptr<zk::Shader> shader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");

    // zk::Ref<zk::Material> m_ParticleMaterial = zk::createRef<zk::Material>({shader});

    std::vector<std::unique_ptr<Particle>> m_Particles;

  private:
    constexpr static double COULOMB_CONSTANT = 8.99e9;
    constexpr static double ELECTRON_CHARGE = 1.60e-19;
    constexpr static float ELECTRON_MASS = 9.11e-31;

    // static constexpr float K = 1.0f;
    static constexpr float K = 300.0f;
    static constexpr float MASS = 1.0f;

    std::unique_ptr<zk::RenderObject> m_Point;
    std::unique_ptr<zk::RenderObject> m_Grid;

    std::vector<glm::vec3> m_Lines;
    std::vector<std::vector<glm::vec3>> allLines;

    int m_Steps = 200;
    int m_SeedCount = 32;
};
