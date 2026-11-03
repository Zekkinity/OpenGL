#pragma once

#include "ZkEngine/World/Lights.hpp"
#include "ZkEngine/World/World.hpp"

namespace zk {

struct FrameData {
    LightData lights;
    CameraData camera;
};

} // namespace zk

