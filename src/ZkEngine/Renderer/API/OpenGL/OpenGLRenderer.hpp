#pragma once

#include <glad/glad.h>
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/Core/RenderView.hpp"
#include "ZkEngine/Renderer/Core/RenderQueue.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLDevice.hpp"

#include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/Passes/ShadowPass.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Passes/SkyboxPass.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Passes/GeometryPass.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Passes/DebugPass.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Passes/PostProcessPass.hpp"

namespace zk {

struct RendererStats {
    uint32_t drawCalls = 0;
    uint32_t vertexCount = 0;
    uint32_t triangleCount = 0;
};

enum class RenderMode {
    Forward,
    Deferred,
    Wireframe,
};

class OpenGLRenderer : public RendererBackend {
  public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void init() override;

    void update() override;
    void submit(const RenderCommand& cmd) override;

    void renderScene(Scene& scene) override;

    void drawLine(const std::vector<glm::vec3>& pts) override;
    void setClearColor(const glm::vec4& color) override;
    void setViewport(const Viewport& viewport) override;
    void resize(uint32_t width, uint32_t height) override;

    RenderDevice& getDevice() override { return m_Device; }

    void debug() override;

    void setSkybox(const std::vector<std::filesystem::path>& tex) override {
        m_SkyboxPass->setSkybox(tex);
    }

    // Draw calls
    void drawElements(GLenum mode, GLsizei count);
    void drawArrays(GLenum mode, GLsizei count);

  private:
    void render(RenderContext& ctx, const RenderView& view);

    void updateCamera(Camera3D& camera);
    // void updateLights();

    void clearQueues();

  private:
    OpenGLDevice m_Device;

    RenderQueue m_RenderQueue;

    RendererStats m_Stats;

    std::unique_ptr<ShadowPass> m_ShadowPass;
    std::unique_ptr<SkyboxPass> m_SkyboxPass;
    std::unique_ptr<GeometryPass> m_GeometryPass;
    std::unique_ptr<DebugPass> m_DebugPass;
    std::unique_ptr<PostProcessPass> m_PostProcessPass;

    std::vector<RenderView> m_Views;

    // Editor
    RenderView m_EditorView;

    std::unique_ptr<Framebuffer> m_EditorFBO;
    Camera3D m_EditorCamera = Camera3D(CameraConfig({.aspect = 512.0f / 512.0f}));
    std::unique_ptr<Framebuffer> m_EditorPostFBO;

    // Scene
    RenderView m_SceneView;
    // Main FBO for scene rendering
    std::unique_ptr<Framebuffer> m_SceneFBO;

    bool m_Debug = true;

    float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    RenderMode m_RenderMode = RenderMode::Forward;

    std::unique_ptr<UniformBuffer> m_CameraUBO;

    bool m_AntiAliasing = true;
};

} // namespace zk
