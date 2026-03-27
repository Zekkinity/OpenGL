#pragma once

#include "ZkEngine/Renderer/Core/RenderContext.hpp"

namespace zk {

class RenderPass {
  public:
    virtual ~RenderPass() = default;
    virtual void execute(const RenderContext& context) = 0;
    virtual void debug() {}
};

} // namespace zk
