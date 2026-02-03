#pragma once

#include <memory>
#include "ZkEngine/Scene/EmptyScene.hpp"
#include "ZkEngine/Scene/Scene.hpp"

namespace zk {

class SceneManager {
  public:
    SceneManager();

    // Maybe send an event instead of immediately switching
    template <typename T>
    void setScene() {
        m_SceneFactory = []() { return std::make_unique<T>(); };
        m_CurrentScene = std::move(m_SceneFactory());
        m_CurrentScene->init();
    }

    Scene& getScene();

    void reloadScene();

    void keepCameraState(bool state) { m_MaintainCameraState = state; }
    bool getKeepCameraState() { return m_MaintainCameraState; }

  private:
    std::unique_ptr<Scene> m_CurrentScene = std::make_unique<EmptyScene>();

    std::function<std::unique_ptr<Scene>()> m_SceneFactory;

    bool m_MaintainCameraState = false;
};

} // namespace zk
