#include "zkpch.hpp"
#include "WorldManager.hpp"

#include "ZkEngine/World/EmptyWorld.hpp"
#include "imgui.h"

namespace zk {

WorldManager::WorldManager(RenderDevice& device, AssetManager& assetManager)
    : m_Device(device), m_AssetManager(assetManager) {
    m_WorldFactory = [this]() {
        auto scene = createScope<EmptyWorld>();
        scene->onInit(&m_AssetManager);
        return scene;
    };

    m_CurrentWorld = m_WorldFactory();
}

void WorldManager::update(float dt) {
    if (m_NextWorld) {
        m_CurrentWorld = std::move(m_NextWorld);
        m_NextWorld = nullptr;
    }

    m_CurrentWorld->onUpdate(dt);
}

World& WorldManager::getWorld() {
    return *m_CurrentWorld;
}

void WorldManager::reloadWorld() {
    // CameraState state = m_CurrentWorld->getCamera()->getState();

    if (m_WorldFactory) {
        m_NextWorld = m_WorldFactory();
    }

    // if (m_MaintainCameraState)
    //     m_CurrentWorld->getCamera()->setState(state);
}

// TODO: Active camera debugging
void WorldManager::debug() {
    ImGui::PushID(static_cast<void*>(m_CurrentWorld.get()));
    m_CurrentWorld->onDebug();
    if (ImGui::Button("Reload World")) {
        reloadWorld();
    }

    ImGui::Checkbox("Keep Camera State", &m_MaintainCameraState);

    ImGui::PopID();
}

} // namespace zk
