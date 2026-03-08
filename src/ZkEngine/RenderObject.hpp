#pragma once

#include "ZkEngine/Renderer/API/OpenGL/Mesh.hpp"
#include "ZkEngine/Renderer/Material.hpp"

namespace zk {

struct RenderObject {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};

}
