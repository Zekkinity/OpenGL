#pragma once

#include "ZkEngine/Renderer/RenderPipeline.hpp"

#include "ZkEngine/Renderer/Framebuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/Passes/SSAOPass.hpp"

namespace zk {

class RenderDevice;
class FrameData;
class BatchRenderer;
class Shader;
class Mesh;
class LightData;

class DeferredRenderer : public RenderPipeline {
  public:
    DeferredRenderer() = default;
    ~DeferredRenderer() = default;

    void init(RenderDevice& device) override;

    uint32_t render(FrameData& frameData, BatchRenderer& batchRenderer) override;

    void resize(uint32_t width, uint32_t height) override;

    void debug() override;

    Framebuffer* getGBuffer() { return m_GBuffer.get(); }

  private:
    void geometryPass(BatchRenderer& batchRenderer);
    void lightingPass(FrameData& frameData, uint32_t ssaoTexture);

  private:
    // Deferred
    Scope<Framebuffer> m_GBuffer;
    Ref<Shader> m_GBufferShader;

    Scope<Framebuffer> m_LightingFBO;
    Ref<Shader> m_LightingShader;

    Scope<SSAOPass> m_SSAOPass;
    bool m_EnableSSAO = false;

    Ref<Shader> m_LightSource;
    Ref<Mesh> m_CubeMesh;

    RenderDevice* m_Device = nullptr;

    // Normal Mapping
    bool m_EnableNormalMapping = true;
};

} // namespace zk
