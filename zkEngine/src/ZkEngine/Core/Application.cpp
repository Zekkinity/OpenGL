#include "zkpch.hpp"

#include "Application.hpp"
#include "ZkEngine/World/WorldManager.hpp"

namespace zk {

void Application::init(WorldManager& worldManager) {
    m_WorldManager = &worldManager;

    onInit();
}

WorldManager& Application::getWorldManager() {
    return *m_WorldManager;
}

World& Application::getWorld() {
    return getWorldManager().getWorld();
}

} // namespace zk
