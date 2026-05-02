#pragma once

namespace zk {

class RenderCommand;

class RenderQueue {
  public:
    RenderQueue() = default;
    ~RenderQueue() = default;

    void submit(const RenderCommand& cmd);

    void clear();

    // Sort transparent objects by farthest first
    void sortTransparent();

    // Sort by shader, material, mesh
    void sortOpaque();

    const std::vector<RenderCommand>& getOpaque() const { return m_OpaqueQueue; }
    const std::vector<RenderCommand>& getTransparent() const { return m_TransparentQueue; }

  private:
    std::vector<RenderCommand> m_OpaqueQueue;
    std::vector<RenderCommand> m_TransparentQueue;

    std::vector<RenderCommand> m_LineQueue;
};

} // namespace zk
