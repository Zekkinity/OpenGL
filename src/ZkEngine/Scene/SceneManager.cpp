#include "SceneManager.hpp"

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

} // namespace zk
