#pragma once

#include "ZkEngine/World/World.hpp"

namespace zk {

class EmptyWorld : public World {
  public:
    void init() override {};
    void update(float dt) override {};
    void debug() override {};
};

} // namespace zk
