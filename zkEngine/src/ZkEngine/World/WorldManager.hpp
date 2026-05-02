#pragma once

#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/World/World.hpp"

namespace zk {

class WorldManager {
  public:
    WorldManager(RenderDevice& device, AssetManager& assetManager);

    void update(float dt);

    void onWindowResize(int width, int height) { m_CurrentWorld->onWindowResize(width, height); }

    // Maybe send an event instead of immediately switching
    template <typename T>
    void setWorld() {
        // TODO: Get skybox working
        // m_Device.setSkybox(nullptr);
        m_WorldFactory = [this]() {
            auto scene = createScope<T>();
            scene->onInit(&m_AssetManager);
            return scene;
        };
        m_NextWorld = m_WorldFactory();
    }

    World& getWorld();

    void reloadWorld();

    void keepCameraState(bool state) { m_MaintainCameraState = state; }
    bool getKeepCameraState() { return m_MaintainCameraState; }

    void debug();

  private:
    RenderDevice& m_Device;
    AssetManager& m_AssetManager;

    Scope<World> m_CurrentWorld;
    Scope<World> m_NextWorld;

    std::function<Scope<World>()> m_WorldFactory;

    bool m_MaintainCameraState = false;
};

} // namespace zk
