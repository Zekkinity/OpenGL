#pragma once

#include "ZkEngine/Renderer/Core/RenderPass.hpp"
#include "ZkEngine/Renderer/Core/RenderQueue.hpp"

namespace zk {

class OpenGLRenderer;

class DebugPass : public RenderPass {
  public:
    DebugPass(OpenGLRenderer* renderer);

    void execute(const RenderContext& ctx) override;
    void debug() override;

  private:
    OpenGLRenderer* m_Renderer;

    std::shared_ptr<Shader> m_DebugNormals;

    bool m_DebugNormalsEnabled = false;
};

} // namespace zk
