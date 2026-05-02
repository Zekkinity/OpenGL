#include "Player.hpp"

#include "ZkEngine/World/World.hpp"

class PhysicsWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    void debug() override;

  private:
    void buildLights();
    void buildFloor();

    void loadCubes();

  private:
    std::shared_ptr<zk::Mesh> m_CubeMesh;

    std::shared_ptr<zk::Texture> m_FloorTexture;
    std::shared_ptr<zk::Material> m_FloorMaterial;

    // Light reciever
    std::shared_ptr<zk::Shader> m_LightingShader;
    std::shared_ptr<zk::Texture> m_BoxTexture;
    std::shared_ptr<zk::Texture> m_BoxSpecularMap;
    std::shared_ptr<zk::Material> m_BoxMaterial;

    // Light source
    std::shared_ptr<zk::Shader> m_LightSourceShader;
    std::shared_ptr<zk::Material> m_LightSourceMaterial;

    glm::vec3 m_LightColor = {1.0f, 1.0f, 1.0f};

    glm::vec3 lightSourcePos = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 lightSourcePos2 = glm::vec3(0.0f, 5.0f, 0.0f);

    std::unique_ptr<Player> m_Player;
    float distance = 40.0f;

  private:
    glm::vec3 m_LightDir = {-0.2f, -1.0f, -0.3f};
    float m_LightLinear = 0.09;
    float m_LightQuadratic = 0.032;

    // FlashLight
    zk::Entity m_SpotLight = createEntity();
    zk::Entity m_DirLight = createEntity();

    zk::Entity m_LightSource = createEntity();
    zk::Entity m_Entity = createEntity();
    float posX = 0.0f;

    int m_CubeCount = 0;
};
