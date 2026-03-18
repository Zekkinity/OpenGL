#include "SceneManager.hpp"
#include "imgui.h"

namespace zk {

SceneManager::SceneManager() {}

Scene& SceneManager::getScene() {
    return *m_CurrentScene;
}

// void SceneManager::setScene(std::unique_ptr<Scene> scene) {
//     m_CurrentScene = std::move(scene);
// }

void SceneManager::reloadScene() {
    CameraState state = m_CurrentScene->getCamera()->getState();

    m_CurrentScene = m_SceneFactory();
    m_CurrentScene->init();

    if (m_MaintainCameraState)
        m_CurrentScene->getCamera()->setState(state);
}

void SceneManager::debug() {
    m_CurrentScene->getCamera()->debug();
    if (ImGui::Button("Reload Scene")) {
        reloadScene();
    }

    ImGui::Checkbox("Keep Camera State", &m_MaintainCameraState);

    m_CurrentScene->getLightSystem().debug();
}

} // namespace zk
