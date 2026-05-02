#pragma once

namespace zk {

class RenderContext;
class RenderView;

class RenderPass {
  public:
    virtual ~RenderPass() = default;
    virtual void execute(const RenderContext& context, const RenderView& view) = 0;
    virtual void debug() {}

    bool m_Enabled = true;
};

} // namespace zk
