#pragma once

namespace zk {

class Scene;
class Registry;
class RenderQueue;
class Camera3D;
struct LightData;

struct RenderContext {
    Scene* scene;
    Registry* registry;
    RenderQueue* queue;
    Camera3D* camera;
    LightData* lights;

    // could do
    // uint32_t inputTexture;
    // Framebuffer* outputFramebuffer;
};

} // namespace zk
