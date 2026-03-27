#pragma once

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Viewport.hpp"

namespace zk {

struct RenderView {
    Camera3D* camera = nullptr;
    Framebuffer* framebuffer = nullptr;
    Viewport viewport;
};

}
