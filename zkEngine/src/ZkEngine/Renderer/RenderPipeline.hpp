#pragma once

namespace zk {

class RenderDevice;
class FrameData;
class BatchRenderer;

class RenderPipeline {
  public:
    RenderPipeline() = default;
    virtual ~RenderPipeline() = default;

    virtual void init(RenderDevice& device) = 0;

    virtual uint32_t render(FrameData& frameData, BatchRenderer& batchRenderer) = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void debug() = 0;
};

} // namespace zk
