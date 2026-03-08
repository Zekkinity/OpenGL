#pragma once

#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class Application {
  public:
    virtual ~Application() = default;

    virtual void onInit() {}
    virtual void onUpdate(float dt) {}
    virtual void onRender(RendererBackend& renderer) {}
    virtual void onImGuiRender() {}
};

} // namespace zk
