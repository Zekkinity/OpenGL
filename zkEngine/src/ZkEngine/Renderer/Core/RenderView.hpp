#pragma once

#include "ZkEngine/Renderer/Viewport.hpp"
#include "ZkEngine/World/World.hpp"

namespace zk {

class Framebuffer;
class CameraData;

enum class ViewType {
    World,
    Editor,
    Shadow,
    Custom
};

struct RenderView {
    CameraData camera;
    Framebuffer* framebuffer = nullptr;
    Viewport viewport;

    ViewType type = ViewType::World;
};

}
