#pragma once

#include "ZkEngine/Renderer/RenderPipeline.hpp"

#include "ZkEngine/Renderer/Framebuffer.hpp"

namespace zk {

struct Light {
    glm::vec3 position;
    glm::vec3 color;
};

class RenderDevice;
class World;
class BatchRenderer;
class DrawContext;
class Shader;
class Mesh;

class DeferredRenderer : public RenderPipeline {
  public:
    DeferredRenderer() = default;
    ~DeferredRenderer() = default;

    void init(RenderDevice& device) override;

    uint32_t render(World& world, BatchRenderer& batchRenderer) override;

    void resize(uint32_t width, uint32_t height) override;

    void debug() override;

  private:
    void geometryPass(const DrawContext& ctx, BatchRenderer& batchRenderer);
    void lightingPass();
    void forwardShadingPass();

    void ssaoPass();

  private:
    // Deferred
    Scope<Framebuffer> m_GBuffer;
    Ref<Shader> m_GBufferShader;

    Scope<Framebuffer> m_LightingFBO;
    Ref<Shader> m_LightingShader;

    Ref<Shader> m_Shader;

    // SSAO
    Scope<Framebuffer> m_SSAOFBO;
    Ref<Shader> m_SSAOShader;

    std::vector<glm::vec3> ssaoKernel;

    Ref<Texture> m_NoiseTexture;

    Scope<Framebuffer> m_SSAOBlurFBO;
    Ref<Shader> m_SSAOBlurShader;

    int m_SSAOKernelSize = 64; // 64 default
    float m_SSAORadius = 0.5f;
    float m_SSAOBias = 0.025f;

    bool m_EnableSSAO = false;
    //

    Ref<Shader> m_LightSource;
    Ref<Mesh> m_CubeMesh;

    RenderDevice* m_Device = nullptr;
};

} // namespace zk
