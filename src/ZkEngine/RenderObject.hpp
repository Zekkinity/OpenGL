#pragma once

#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/Mesh.hpp"
#include "ZkEngine/Renderer/Material.hpp"

namespace zk {

struct RenderObject {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    RenderObject(std::shared_ptr<Mesh> m, std::shared_ptr<Material> mat)
        : mesh(std::move(m)), material(std::move(mat)) {
        ZK_CORE_ASSERT(mesh, "RenderObject requires a mesh");
        ZK_CORE_ASSERT(material, "RenderObject requires a material");
    }
};

} // namespace zk
