// #pragma once
//
// #include <glad/glad.h>
// #include "tracy/TracyOpenGL.hpp"
//
// #include "ZkEngine/Renderer/RendererBackend.hpp"
//
// #include "ZkEngine/Renderer/Core/RenderView.hpp"
//
// #include "ZkEngine/Renderer/API/OpenGL/OpenGLDevice.hpp"
//
// namespace zk {
//
// struct RendererStats {
//     uint32_t drawCalls = 0;
//     uint32_t indexCount = 0;
//     uint32_t triangleCount = 0;
// };
//
// enum class RenderMode {
//     Forward,
//     Deferred,
//     Wireframe,
// };
//
// class RenderContext;
// class UniformBuffer;
//
// class ShadowPass;
// class SkyboxPass;
// class GeometryPass;
// class DebugPass;
// class BloomPass;
// class PostProcessPass;
//
// struct Batch;
//
// class OpenGLRenderer : public RendererBackend {
//   public:
//     OpenGLRenderer();
//     ~OpenGLRenderer();
//
//     void init() override;
//
//     void update() override;
//
//     void renderWorld(World& world) override;
//
//     void drawLine(const std::vector<glm::vec3>& pts) override;
//
//     void setClearColor(const glm::vec4& color) override;
//     void setViewport(const Viewport& viewport) override;
//     void resize(uint32_t width, uint32_t height) override;
//
//     RenderDevice& getDevice() override { return m_Device; }
//
//     Framebuffer* getWorldFBO() override { return m_WorldFBO.get(); }
//     Framebuffer* getEditorFBO() override { return m_EditorFBO.get(); }
//
//     void debug() override;
//
//     void setSkybox(const Ref<Texture>& tex) override;
//
//     // Draw calls
//     void drawElements(uint32_t mode, uint32_t count);
//     void drawArrays(uint32_t mode, uint32_t count);
//     void drawMesh(const Ref<Mesh>& mesh) override;
//     void drawBatch(const Batch& batch) override;
//
//     void drawInstanced(int count, int instanceCount);
//
//     void renderScreenQuad();
//
//   private:
//     void renderForward(World& world);
//
//     void render(RenderContext& ctx, const RenderView& view);
//
//     void updateCamera(const CameraData& camera);
//     void updateLights(const LightData& data);
//
//   private:
//     OpenGLDevice m_Device;
//
//     RendererStats m_Stats;
//
//     Scope<ShadowPass> m_ShadowPass;
//     Scope<SkyboxPass> m_SkyboxPass;
//     Scope<GeometryPass> m_GeometryPass;
//     Scope<DebugPass> m_DebugPass;
//     Scope<BloomPass> m_BloomPass;
//     Scope<PostProcessPass> m_PostProcessPass;
//
//     Scope<Framebuffer> m_EditorFBO;
//     Scope<Framebuffer> m_EditorPostFBO;
//
//     // Main FBO for scene rendering
//     Scope<Framebuffer> m_WorldFBO;
//
//     bool m_Debug = true;
//
//     float m_ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
//
//     RenderMode m_RenderMode = RenderMode::Deferred;
//
//     Scope<UniformBuffer> m_CameraUBO;
//     Scope<UniformBuffer> m_LightsUBO;
//
//     bool m_AntiAliasing = true;
//
//     bool m_ActiveEditor = false;
//
//     // Fullscreen quad
//     uint32_t m_QuadVAO = 0;
//     uint32_t m_QuadVBO = 0;
// };
//
// } // namespace zk
