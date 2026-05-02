#include "ZkEngine/World/World.hpp"

#include "Player.hpp"

class SimpleWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    void debug() override;

  private:
    std::shared_ptr<zk::Material> redMaterial;

    zk::Entity backpack = createEntity();

    std::unique_ptr<Player> m_Player;

    zk::Entity rigidBody;

    float alpha = 0.0f;
};
