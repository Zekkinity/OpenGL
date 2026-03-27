#pragma once

#include "ZkEngine/Core/Engine.hpp"
#include "ZkEngine/ECS/Entity.hpp"

namespace zk {

inline SceneManager& GetSceneManager() {
    return Engine::Get()->getSceneManager();
}

inline Scene& GetCurrentScene() {
    return GetSceneManager().getScene();
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

inline void LockCursor(bool locked) {
    Engine::Get()->lockCursor(locked);
}

inline bool IsCursorLocked() {
    return Engine::Get()->isCursorLocked();
}

// Maybe recieve light type
// LightType::Point || LightType::Directional || LightType::Spot
// Maybe recieve light color
// Maybe recieve light position
// inline Entity CreateLight(LightType type, glm::vec3 color, glm::vec3 position) {}
inline Entity CreateLight() {
    Entity light = GetRegistry().createEntity();
    light.addPointLight();
    // light.addComponent<zk::PointLight>(zk::PointLight{
    //     .linear = 0.09f,
    //     .quadratic = 0.003f,
    // });

    return light;
}

inline Entity createEntity() {
    return GetRegistry().createEntity();
}

inline std::filesystem::path asset(const std::filesystem::path& path) {
    return Engine::Get()->getAssetManager().asset(path);
}

namespace Asset {

inline std::shared_ptr<Texture> loadTexture(const std::filesystem::path& path, const TextureSpecs& specs = {}) {
    return Engine::Get()->getAssetManager().loadTexture(path, specs);
}

inline std::shared_ptr<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces) {
    return Engine::Get()->getAssetManager().loadTextureCube(faces);
}

inline std::shared_ptr<Shader> loadShader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath = "") {
    return Engine::Get()->getAssetManager().loadShader(vertexPath, fragmentPath, geometryPath);
}

inline std::shared_ptr<Material> loadMaterial(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture = nullptr, std::shared_ptr<Texture> specularMap = nullptr) {
    return Engine::Get()->getAssetManager().loadMaterial(shader, texture, specularMap);
}

} // namespace Asset

} // namespace zk
