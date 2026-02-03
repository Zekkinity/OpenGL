#include "AssetManager.hpp"

std::filesystem::path AssetManager::s_AssetRoot;

void AssetManager::setRootPath(const std::filesystem::path& path) {
    s_AssetRoot = path;
}

std::filesystem::path AssetManager::asset(const std::filesystem::path& relative) {
    return s_AssetRoot / relative;
}
