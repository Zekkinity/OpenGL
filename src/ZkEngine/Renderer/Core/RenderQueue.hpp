#pragma once

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"
#include <algorithm>

namespace zk {

class RenderQueue {
  public:
    RenderQueue() = default;
    ~RenderQueue() = default;

    void submit(const RenderCommand& cmd) {
        ZK_CORE_ASSERT(cmd.mesh, "RenderCommand must have a mesh");
        ZK_CORE_ASSERT(cmd.material, "RenderCommand must have a material");

        if (cmd.material->isTransparent())
            m_TransparentQueue.push_back(cmd);
        else
            m_OpaqueQueue.push_back(cmd);
    }

    void clear() {
        m_OpaqueQueue.clear();
        m_TransparentQueue.clear();
    }

    // Sort transparent objects by farthest first
    void sortTransparent() {
        std::sort(
            m_TransparentQueue.begin(),
            m_TransparentQueue.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                return a.distanceToCamera > b.distanceToCamera;
            });
    }

    const std::vector<RenderCommand>& getOpaque() const { return m_OpaqueQueue; }
    const std::vector<RenderCommand>& getTransparent() const { return m_TransparentQueue; }

  private:
    std::vector<RenderCommand> m_OpaqueQueue;
    std::vector<RenderCommand> m_TransparentQueue;

    std::vector<RenderCommand> m_LineQueue;
};

} // namespace zk
