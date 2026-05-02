#include "ZkEngine/World/World.hpp"

#include "ZkEngine/Core/AppAcess.hpp"

class MinecraftWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    void debug() override;

  private:
    void buildDirt();

    void buildFloor();

  private:
    std::shared_ptr<zk::Mesh> m_CubeMesh;
    std::shared_ptr<zk::Material> m_Material;

    zk::Ref<zk::Shader> m_LightShader;

    int n = 1;
};
