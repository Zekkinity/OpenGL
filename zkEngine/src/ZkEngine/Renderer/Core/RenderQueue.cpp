#include "zkpch.hpp"

#include "ZkEngine/Renderer/Core/RenderQueue.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"

#include "ZkEngine/Renderer/Material.hpp"

namespace zk {

void RenderQueue::submit(const RenderCommand& cmd) {
    ZK_CORE_ASSERT(cmd.mesh, "RenderCommand must have a mesh");
    ZK_CORE_ASSERT(cmd.material, "RenderCommand must have a material");

    if (cmd.material->isTransparent())
        m_TransparentQueue.push_back(cmd);
    else
        m_OpaqueQueue.push_back(cmd);
}

void RenderQueue::clear() {
    m_OpaqueQueue.clear();
    m_TransparentQueue.clear();
}

// Sort transparent objects by farthest first
void RenderQueue::sortTransparent() {
    std::sort(
        m_TransparentQueue.begin(),
        m_TransparentQueue.end(),
        [](const RenderCommand& a, const RenderCommand& b) {
            return a.distanceToCamera > b.distanceToCamera;
        });
}

// Sort by shader, material, mesh
void RenderQueue::sortOpaque() {
    std::sort(m_OpaqueQueue.begin(), m_OpaqueQueue.end(), [](const RenderCommand& a, const RenderCommand& b) {
        Shader* shaderA = &a.material->getShader();
        Shader* shaderB = &b.material->getShader();

        if (shaderA != shaderB)
            return shaderA < shaderB;

        if (a.material.get() != b.material.get())
            return a.material.get() < b.material.get();

        return a.mesh.get() < b.mesh.get();
    });
}

} // namespace zk
