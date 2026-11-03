#pragma once

#include "ZkEngine/Core/Base.hpp"

#include <glm/glm.hpp>

#include "ZkEngine/Renderer/API/OpenGL/Passes/BloomPass.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Passes/ShadowPass.hpp"
#include "ZkEngine/Renderer/BatchRenderer.hpp"
#include "ZkEngine/Renderer/Core/RenderQueue.hpp"
#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/RenderPipeline.hpp"
#include "ZkEngine/Renderer/Viewport.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"

enum class RenderAPI { OpenGL, Vulkan };

namespace zk {

class World;
struct Batch;
struct Frustum;

class UniformBuffer;
class CameraData;
class LightData;

class RendererBackend {
  public:
    RendererBackend(RenderAPI api);
    ~RendererBackend();

    static Scope<RendererBackend> create(RenderAPI api);
    void init();

    // virtual void init() = 0;
    // virtual void update() = 0;

    void beginFrame(const World& world);
    void render(World& world);
    void endFrame();

    // virtual void renderWorld(World& scene) = 0;
    //
    // virtual void setClearColor(const glm::vec4& color) = 0;
    // virtual void setViewport(const Viewport& viewport) = 0;
    //
    // virtual void debug() = 0;
    //
    // virtual void resize(uint32_t width, uint32_t height) = 0;
    //
    // virtual void setSkybox(const Ref<Texture>& cubeMap) {}
    //
    // virtual Framebuffer* getWorldFBO() { return nullptr; }
    // virtual Framebuffer* getEditorFBO() { return nullptr; }

    void debug();

    void resize(uint32_t width, uint32_t height);

    RenderDevice& getDevice() { return *m_Device; }

    RenderAPI getAPI() const { return m_API; }

  private:
    void getSceneData(const World& world);
    void updateCamera(const CameraData& camera);
    void updateLights(const LightData& data);

    bool isVisible(const Frustum& frustum, const AABB& box) const;

    void postProcessPass(uint32_t sceneColor, uint32_t bloomTexture);

  protected:
    RenderQueue m_RenderQueue;

    Scope<BatchRenderer> m_BatchRenderer;

  private:
    RenderAPI m_API = RenderAPI::OpenGL;

    Scope<RenderDevice> m_Device;

    Scope<RenderPipeline> m_Pipeline;

    Scope<UniformBuffer> m_CameraUBO;
    Scope<UniformBuffer> m_LightsUBO;

    bool m_FrustumCulling = true;

    Scope<ShadowPass> m_ShadowPass;

    Scope<BloomPass> m_BloomPass;
    // postprocess
    Ref<Shader> m_PostProcessShader;

    bool m_GammaCorrection = true;
    bool m_Kernel = true;
    bool m_HDR = true;

    float m_Exposure = 1.0f;

    bool m_Bloom = false;
    float m_BloomStrength = 0.04f;
};

} // namespace zk
