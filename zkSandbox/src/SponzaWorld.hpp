#pragma once

#include "ZkEngine/World/World.hpp"

class SponzaWorld : public zk::World {
  public:
    void init() override;
    void update(float dt) override;

    void debug() override;

  private:

};

