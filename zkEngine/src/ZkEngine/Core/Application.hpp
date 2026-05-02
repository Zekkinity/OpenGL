#pragma once

#include "ZkEngine/World/WorldManager.hpp"

namespace zk {

class Application {
  public:
    Application() = default;
    virtual ~Application() = default;

    void init(WorldManager& worldManager);

    virtual void onInit() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onRender(RendererBackend& renderer) = 0;
    virtual void onImGuiRender() = 0;

    WorldManager& getWorldManager();
    World& getWorld();

  private:
    WorldManager* m_WorldManager = nullptr;
};

} // namespace zk
