#pragma once

#include "ZkEngine/Scene/Scene.hpp"

namespace zk {

class EmptyScene : public Scene {
  public:
    void init() override {};
    void update(float dt) override {};
    void submit(RendererBackend& renderer) const override {};
    void debug() override {};
};

} // namespace zk
