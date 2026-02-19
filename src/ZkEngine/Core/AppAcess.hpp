#pragma once

#include "ZkEngine/Core/Engine.hpp"
#include "ZkEngine/Renderer/Renderer.hpp"
#include "ZkEngine/Scene/Lights.hpp"
#include "ZkEngine/Scene/Entity.hpp"

namespace zk {

inline SceneManager& GetSceneManager() {
    return Engine::Get()->getSceneManager();
}

inline Scene& GetCurrentScene() {
    return GetSceneManager().getScene();
}
inline LightSystem& GetLightSystem() {
    return GetCurrentScene().getLightSystem();
}

inline Window& GetWindow() {
    return Engine::Get()->getWindow();
}

inline void ToggleDebugWindow() {
    Engine::Get()->toggleDebugWindow();
}

inline Entity CreateLight() {
    Entity light = GetCurrentScene().createEntity();
    light.addComponent<zk::PointLight>(zk::PointLight{
        .ambient = {glm::vec3(1.0f, 0.0f, 0.0f) * glm::vec3(0.8f)},
        // .ambient = ambientColor,
        .diffuse = {glm::vec3(1.0f, 0.0f, 0.0f)},
        .specular = glm::vec3(1.0f),
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.003f,

    });

    return light;
}

} // namespace zk
