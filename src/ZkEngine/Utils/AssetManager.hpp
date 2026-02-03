#pragma once

#include <filesystem>

class AssetManager {
public:
    static void setRootPath(const std::filesystem::path& path);
    static std::filesystem::path asset(const std::filesystem::path& path);
private:
    static std::filesystem::path s_AssetRoot;
};
