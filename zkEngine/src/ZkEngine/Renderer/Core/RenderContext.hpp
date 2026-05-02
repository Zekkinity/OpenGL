#pragma once

namespace zk {

class World;
class RenderQueue;
class Camera;
struct LightData;

struct RenderContext {
    World* world = nullptr;
    RenderQueue* queue = nullptr;
    LightData* lights = nullptr;

    uint32_t sceneColor = 0;
    uint32_t sceneDepth = 0;
    uint32_t brightColor = 0;
    uint32_t bloomBlur = 0;
};

} // namespace zk
