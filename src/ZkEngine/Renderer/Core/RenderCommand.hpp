#pragma once

#include "ZkEngine/Renderer/API/OpenGL/Mesh.hpp"
#include "ZkEngine/Renderer/Material.hpp"

namespace zk {

struct RenderCommand {
    Mesh* mesh;
    Material* material;
    glm::mat4 modelMatrix;
    float distanceToCamera;
};

} // namespace zk
