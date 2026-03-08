#pragma once

#include "ZkEngine/Core/Engine.hpp"
#include "ZkEngine/Renderer/Renderer.hpp"
#include "ZkEngine/Scene/Lights.hpp"
#include "ZkEngine/ECS/Entity.hpp"

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
inline Registry& GetRegistry() {
    return GetCurrentScene().getRegistry();
}

inline Window& GetWindow() {
    return Engine::Get()->getWindow();
}

inline void ToggleDebugWindow() {
    Engine::Get()->toggleDebugWindow();
}

// Maybe recieve light type
// LightType::Point || LightType::Directional || LightType::Spot
// Maybe recieve light color
// Maybe recieve light position
// inline Entity CreateLight(LightType type, glm::vec3 color, glm::vec3 position) {}
inline Entity CreateLight() {
    Entity light = GetRegistry().createEntity();
    light.addComponent<zk::PointLight>(zk::PointLight{
        .linear = 0.09f,
        .quadratic = 0.003f,
    });

    return light;
}

inline Entity createEntity() {
    return GetRegistry().createEntity();
}

inline std::filesystem::path asset(const std::filesystem::path& path) {
    return Engine::Get()->getAssetManager().asset(path);
}

namespace Asset {

inline std::shared_ptr<Texture> loadTexture(const std::filesystem::path& path, TextureFiltering filtering = TextureFiltering::NEAREST) {
    return Engine::Get()->getAssetManager().loadTexture(path, filtering);
}

inline std::shared_ptr<Shader> loadShader(
    const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    return Engine::Get()->getAssetManager().loadShader(vertexPath, fragmentPath);
}

inline std::shared_ptr<Material> loadMaterial(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture = nullptr, std::shared_ptr<Texture> specularMap = nullptr) {
    return Engine::Get()->getAssetManager().loadMaterial(shader, texture, specularMap);
}

} // namespace Asset

} // namespace zk
