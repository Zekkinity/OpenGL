#pragma once

#include "ZkEngine/Core/Engine.hpp"

namespace zk {

// inline Renderer& Renderer = Engine::Get()->getRenderer();

// inline Renderer& renderer() {
//     return Engine::Get()->getRenderer();
// }

inline SceneManager& GetSceneManager() {
    return Engine::Get()->getSceneManager();
}

inline Scene& GetCurrentScene() { return GetSceneManager().getScene(); }

inline Window& GetWindow() { return Engine::Get()->getWindow(); }

inline void ToggleDebugWindow() { Engine::Get()->toggleDebugWindow(); }

} // namespace zk
