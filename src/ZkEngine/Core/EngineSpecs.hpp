#pragma once

#include <cstdint>
#include <string>

namespace zk {

struct EngineSpecs {
    std::string title = "OpenGL Engine";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool vsync = true;
};

} // namespace zk
