#include "OpenGLRenderer.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Debug/OpenGLDebug.hpp"
#include "ZkEngine/Renderer/CameraUBO.hpp"

#include "ZkEngine/Renderer/Core/RenderContext.hpp"
#include "imgui.h"

namespace zk {

// NOTE:
// Make a UBO for this
// - bind 1: Lights
// - bind 2: Materials

/*
     TODO:
     - Sort Render commands by shader, material, mesh
     - Do Instance rendering
     - Do Batch rendering
     - Implement Lights UBO
     - Implement Frustum & Occlusion Culling
     - Implement outline stencil
*/
OpenGLRenderer::OpenGLRenderer() : RendererBackend(RenderAPI::OpenGL) {}

OpenGLRenderer::~OpenGLRenderer() {}

void OpenGLRenderer::init() {
    OpenGLDebug::init(m_Debug);

    m_ShadowPass = std::make_unique<ShadowPass>(this);
    m_SkyboxPass = std::make_unique<SkyboxPass>(this);
    m_GeometryPass = std::make_unique<GeometryPass>(this);
    m_DebugPass = std::make_unique<DebugPass>(this);

    m_PostProcessPass = std::make_unique<PostProcessPass>();

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // 3D
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Seamless
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Anti Aliasing
    glEnable(GL_MULTISAMPLE);

    m_CameraUBO = std::make_unique<UniformBuffer>(sizeof(CameraUBO), 0);
    // m_LightsUBO = std::make_unique<UniformBuffer>(sizeof(LightsUBO), 1);

    // Frame buffers
    FramebufferSpec specs;
    specs.width = 1920;
    specs.height = 1080;
    specs.attachments = {
        {TextureFormat::RGBA8, TextureType::Texture2D},
        {TextureFormat::Depth, TextureType::Texture2D},
    };
    m_SceneFBO = m_Device.createFramebuffer(specs);

    FramebufferSpec editorSpecs;
    editorSpecs.width = 512;
    editorSpecs.height = 512;
    editorSpecs.attachments = {
        {TextureFormat::RGBA8, TextureType::Texture2D},
        {TextureFormat::Depth, TextureType::Texture2D},
    };

    m_EditorFBO = m_Device.createFramebuffer(editorSpecs);

    m_EditorPostFBO = m_Device.createFramebuffer(editorSpecs);

    m_EditorView = RenderView{
        .camera = &m_EditorCamera,
        .framebuffer = m_EditorFBO.get(),
        .viewport = {0, 0, 512, 512},
    };
}

void OpenGLRenderer::update() {
    m_Stats = {};

    if (m_AntiAliasing)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);

    switch (m_RenderMode) {
    case RenderMode::Forward:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case RenderMode::Deferred:
        break;

    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;

    default:
        break;
    }
}

void OpenGLRenderer::submit(const RenderCommand& cmd) {
    m_RenderQueue.submit(cmd);
}

void OpenGLRenderer::renderScene(Scene& scene) {
    m_SceneView.framebuffer = m_SceneFBO.get();
    m_SceneView.viewport = {0, 0, m_SceneFBO->getWidth(), m_SceneFBO->getHeight()};
    m_SceneView.camera = scene.getCamera();

    scene.updateModelSystem(*this);
    // Fill Render Queue
    scene.submit(*this);

    LightData lightData = scene.getLights();

    RenderContext ctx;
    ctx.scene = &scene;
    ctx.registry = &scene.getRegistry();
    ctx.queue = &m_RenderQueue;
    ctx.camera = scene.getCamera();
    ctx.lights = &lightData;

    m_ShadowPass->execute(ctx);

    // for (auto& view : m_Views) {
    //     render(ctx, view);
    //     m_PostProcessor->process(view.framebuffer->getColorAttachment(0), nullptr);
    // }

    {
        // Render Scene
        render(ctx, m_SceneView);
        m_PostProcessPass->setInput(m_SceneView.framebuffer->getColorAttachment(0));
        m_PostProcessPass->setOutput(nullptr);
        m_PostProcessPass->execute(ctx);
    }

    {
        // Render Editor
        render(ctx, m_EditorView);
        m_PostProcessPass->setInput(m_EditorView.framebuffer->getColorAttachment(0));
        m_PostProcessPass->setOutput(m_EditorPostFBO.get());
        m_PostProcessPass->execute(ctx);
    }

    clearQueues();
}

void OpenGLRenderer::render(RenderContext& ctx, const RenderView& view) {
    ctx.camera = view.camera;

    view.framebuffer->bind();
    glEnable(GL_DEPTH_TEST);

    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

    setViewport(view.viewport);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCamera(*view.camera);

    m_SkyboxPass->execute(ctx);
    m_GeometryPass->execute(ctx);
    m_DebugPass->execute(ctx);

    view.framebuffer->unbind();
}

void OpenGLRenderer::drawLine(const std::vector<glm::vec3>& pts) {
    m_GeometryPass->drawLine(pts);
}

void OpenGLRenderer::setClearColor(const glm::vec4& color) {
    m_ClearColor[0] = color.r;
    m_ClearColor[1] = color.g;
    m_ClearColor[2] = color.b;
    m_ClearColor[3] = color.a;

    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRenderer::resize(uint32_t width, uint32_t height) {
    m_SceneFBO->resize(width, height);
    m_SceneView.viewport.width = width;
    m_SceneView.viewport.height = height;
    // glViewport(0, 0, width, height);
}

void OpenGLRenderer::setViewport(const Viewport& viewport) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void OpenGLRenderer::updateCamera(Camera3D& camera) {
    CameraUBO data;
    data.view = camera.getView();
    data.proj = camera.getProjection();
    data.viewPos = glm::vec4(camera.getPosition(), 1.0f);
    m_CameraUBO->setData(&data, sizeof(CameraUBO));
}

void OpenGLRenderer::debug() {
    ImGui::Text("Draw Calls: %d", m_Stats.drawCalls);
    ImGui::Text("Triangle Count: %d", m_Stats.triangleCount);
    ImGui::Text("Vertex Count: %d", m_Stats.vertexCount);

    ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);

    ImGui::Combo(
        "Render Mode", reinterpret_cast<int*>(&m_RenderMode), "Forward\0Deferred\0Wireframe\0\0");
    ImGui::Checkbox("Anti Aliasing", &m_AntiAliasing);
    m_DebugPass->debug();

    m_GeometryPass->debug();

    m_PostProcessPass->debug();

    m_EditorCamera.debug();

    ImGui::Begin("Scene View");
    ImVec2 size = ImGui::GetContentRegionAvail();
    static ImVec2 lastSize = {0, 0};

    if (size.x > 0 && size.y > 0 && (size.x != lastSize.x || size.y != lastSize.y)) {
        m_EditorPostFBO->resize((uint32_t)size.x, (uint32_t)size.y);
        lastSize = size;
    }

    uint32_t tex = m_EditorPostFBO->getColorAttachment(0);

    ImGui::Image(
        (ImTextureID)(uintptr_t)tex,
        ImVec2(m_EditorPostFBO->getWidth(), m_EditorPostFBO->getHeight()),
        ImVec2(0, 1),
        ImVec2(1, 0));

    ImGui::End();

    m_ShadowPass->debug();
}

void OpenGLRenderer::clearQueues() {
    // Renderer queues
    m_RenderQueue.clear();

    m_GeometryPass->clear();
}

void OpenGLRenderer::drawArrays(GLenum mode, GLsizei count) {
    if (count <= 0) {
        ZK_CORE_ERROR("Invalid index count: {}", count);
        return;
    }

    glDrawArrays(mode, 0, count);
    m_Stats.drawCalls++;
    if (mode == GL_TRIANGLES)
        m_Stats.triangleCount += count / 3;
    m_Stats.vertexCount += count;
}

void OpenGLRenderer::drawElements(GLenum mode, GLsizei count) {
    if (count <= 0) {
        ZK_CORE_ERROR("Invalid index count: {}", count);
        return;
    }

    glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
    m_Stats.drawCalls++;
    if (mode == GL_TRIANGLES)
        m_Stats.triangleCount += count / 3;
    m_Stats.vertexCount += count;
}

} // namespace zk
