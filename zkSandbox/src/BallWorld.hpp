#include "ZkEngine/World/World.hpp"

class BallWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    void debug() override;

  private:
    glm::vec3 p1 = glm::vec3(0.0f);
    glm::vec3 p2 = glm::vec3(0.0f);

    float r = 0.0f;
    float angle = 0.0f;
};
