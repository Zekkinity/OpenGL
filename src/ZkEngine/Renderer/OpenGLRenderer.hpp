#pragma once

#include "ZkEngine/Renderer/Core/RenderQueue.hpp"
#include "ZkEngine/Renderer/API/OpenGL/PostProcessor.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"
#include "ZkEngine/Renderer/TextureCube.hpp"
#include "ZkEngine/Scene/Lights.hpp"
#include "ZkEngine/Scene/Scene.hpp"

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

struct RenderView {
    Camera3D* camera = nullptr;
    Framebuffer* framebuffer = nullptr;
    Viewport viewport;
};

struct GPUPointLight {
    PointLight light;
    glm::vec3 position;
};

struct GPUSpotLight {
    SpotLight light;
    glm::vec3 position;
};

struct LightData {
    std::vector<GPUPointLight> pointLights;
    std::vector<GPUSpotLight> spotLights;
    std::vector<DirectionalLight> dirLights;
};

class OpenGLRenderer : public RendererBackend {
  public:
    void init() override;

    void update() override;
    void submit(const RenderCommand& cmd) override;

    void renderScene(Scene& scene) override;

    void drawLine(const std::vector<glm::vec3>& pts) override;
    void setClearColor(const glm::vec4& color) override;
    void setViewport(const Viewport& viewport) override;
    void resize(uint32_t width, uint32_t height) override;

    void debug() override;

    void updateCamera(Camera3D& camera);

    void setSkybox(const std::vector<std::filesystem::path>& tex) override {
        m_CubemapTexture = TextureCube::load(tex);
    }

  private:
    LightData extractLights(const Scene& scene);
    void uploadLightsToShader(Shader& shader, const LightData& data);

    void geometryPass(const LightData& lightData);
    void skyboxPass(Camera3D& scene);
    void debugNormalsPass();

    void clearQueues();

    void draw(const RenderCommand& cmd);

    // Draw calls
    void drawElements(GLenum mode, GLsizei count);
    void drawArrays(GLenum mode, GLsizei count);

    // Shadow pass
    void updateShadowMap();
    void updateShadowCubeMap();

    void render(const Scene& scene, const RenderView& view, const LightData& lightData);

    void bindGlobalUniforms(Shader& shader);

  private:
    RenderQueue m_RenderQueue;
    PostProcessor m_PostProcessor;

    RendererStats m_Stats;

    Shader* m_CurrentShader = nullptr;

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

    bool m_EnableTextures = true;

    uint32_t m_LineVAO = 0, m_LineVBO = 0;
    struct LineStripCmd {
        std::vector<glm::vec3> points;
    };

    std::vector<LineStripCmd> m_LineQueue;

    std::unique_ptr<UniformBuffer> m_CameraUBO;

    std::unique_ptr<Framebuffer> m_ShadowMapFBO;
    std::shared_ptr<Shader> m_ShadowShader =
        Asset::loadShader("shaders/ShadowMap.vert", "shaders/ShadowMap.frag");

    std::unique_ptr<Framebuffer> m_DepthCubeMapFBO;
    std::shared_ptr<Shader> m_DepthCubeMapShader = Asset::loadShader(
        "shaders/ShadowCubeMap.vert", "shaders/ShadowCubeMap.frag", "shaders/ShadowCubeMap.geom");

    glm::vec3 m_LightPos = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_FarPlane = 100.0f;

    glm::mat4 m_LightSpaceMatrix = glm::mat4(1.0f);
    glm::vec3 m_LightDir = glm::vec3(0.0f, 0.0f, 0.0f);

    const uint32_t SHADOW_WIDTH = 1024;
    const uint32_t SHADOW_HEIGHT = 1024;

    uint32_t m_EmptyVAO = 0;
    std::shared_ptr<Shader> m_SkyboxShader =
        Asset::loadShader("shaders/Sky.vert", "shaders/Sky.frag");
    std::shared_ptr<Texture> m_CubemapTexture;

    bool m_AntiAliasing = true;
    bool m_DebugNormalsEnabled = false;
    std::shared_ptr<Shader> m_DebugNormals = Asset::loadShader(
        "shaders/VisualizeNormals.vert",
        "shaders/VisualizeNormals.frag",
        "shaders/VisualizeNormals.geom");

    // Material shininss
    float m_Shininess = 64.0f;
    bool m_EnableBlinn = true;
};

} // namespace zk
