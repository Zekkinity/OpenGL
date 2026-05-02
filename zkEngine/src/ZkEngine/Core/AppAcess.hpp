#pragma once

#include "ZkEngine/Core/Engine.hpp"

#include "ZkEngine/World/Entity.hpp"

namespace zk {

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

namespace Asset {

inline Ref<Texture> loadTexture(const std::filesystem::path& path, const TextureSpecs& specs = {}) {
    return Engine::Get()->getAssetManager().loadTexture(path, specs);
}

inline Ref<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces) {
    return Engine::Get()->getAssetManager().loadTextureCube(faces);
}

inline Ref<Shader> loadShader(
    std::string_view vertexPath,
    std::string_view fragmentPath,
    std::string_view geometryPath = "") {
    return Engine::Get()->getAssetManager().loadShader(vertexPath, fragmentPath, geometryPath);
}

inline Ref<Material> loadMaterial(const MaterialSpecs& specs) {
    return Engine::Get()->getAssetManager().loadMaterial(specs);
}

inline Ref<Model> loadModel(const std::filesystem::path& path) {
    return Engine::Get()->getAssetManager().loadModel(path);
}

inline Ref<Texture> getWhiteTexture() {
    return Engine::Get()->getAssetManager().getWhiteTexture();
}

} // namespace Asset

} // namespace zk
