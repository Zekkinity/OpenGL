#include "OpenGLRenderer.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/API/OpenGL/Debug/OpenGLDebug.hpp"
#include "ZkEngine/Renderer/CameraUBO.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
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
     - Implement Frustum Culling
     - Implement outline stencil
*/
void OpenGLRenderer::init() {
    OpenGLDebug::init(m_Debug);

    // For skybox
    glGenVertexArrays(1, &m_EmptyVAO);

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
    m_SceneFBO = Framebuffer::create(specs);

    // Shadow FBO
    FramebufferSpec shadowSpecs;
    shadowSpecs.width = SHADOW_WIDTH;
    shadowSpecs.height = SHADOW_HEIGHT;
    shadowSpecs.attachments = {
        {TextureFormat::Depth32F, TextureType::Texture2D},
    };
    m_ShadowMapFBO = Framebuffer::create(shadowSpecs);

    // Line stuff
    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);

    FramebufferSpec depthCubeMap;
    depthCubeMap.width = SHADOW_WIDTH;
    depthCubeMap.height = SHADOW_HEIGHT;
    depthCubeMap.attachments = {
        {TextureFormat::Depth32F, TextureType::TextureCubeMap},
    };
    m_DepthCubeMapFBO = Framebuffer::create(depthCubeMap);

    FramebufferSpec editorSpecs;
    editorSpecs.width = 512;
    editorSpecs.height = 512;
    editorSpecs.attachments = {
        {TextureFormat::RGBA8, TextureType::Texture2D},
        {TextureFormat::Depth, TextureType::Texture2D},
    };
    m_EditorFBO = Framebuffer::create(editorSpecs);

    m_EditorPostFBO = Framebuffer::create(editorSpecs);

    m_EditorView = RenderView{
        .camera = &m_EditorCamera,
        .framebuffer = m_EditorFBO.get(),
        .viewport = {0, 0, 512, 512},
    };
}

void OpenGLRenderer::update() {
    m_Stats = {};

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

    if (m_AntiAliasing)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
}

void OpenGLRenderer::submit(const RenderCommand& cmd) {
    m_RenderQueue.submit(cmd);
}

void OpenGLRenderer::renderScene(Scene& scene) {
    // std::chrono::high_resolution_clock::time_point start =
    //     std::chrono::high_resolution_clock::now();

    // Fill Render Queue
    scene.submit(*this);

    LightData lightData = extractLights(scene);

    updateShadowCubeMap();
    // updateShadowMap();

    m_SceneView.framebuffer = m_SceneFBO.get();
    m_SceneView.viewport = {0, 0, m_SceneFBO->getWidth(), m_SceneFBO->getHeight()};
    m_SceneView.camera = scene.getCamera();

    {
        render(scene, m_SceneView, lightData);
        m_PostProcessor.process(m_SceneView.framebuffer->getColorAttachment(0), nullptr);
    }

    {
        // Render Editor
        render(scene, m_EditorView, lightData);
        m_PostProcessor.process(
            m_EditorView.framebuffer->getColorAttachment(0), m_EditorPostFBO.get());
    }

    clearQueues();

    // std::chrono::high_resolution_clock::time_point end =
    // std::chrono::high_resolution_clock::now();
    //
    // auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // std::cout << "Render time: " << time << "ms\n";
}

/*
    TODO:
    - Drawing order matters for blending and stencil
    - Blending
        - Draw all opaque objects first
        - Sort all transparent objects
        - Draw all transparent objects in sorted order
*/

// Normal rendering pass
void OpenGLRenderer::geometryPass(const LightData& lightData) {
    // glDisable(GL_BLEND);
    // glDepthMask(GL_TRUE);

    for (const auto& cmd : m_RenderQueue.getOpaque()) {
        Shader& shader = cmd.material->getShader();

        if (&shader != m_CurrentShader) {
            shader.bind();
            bindGlobalUniforms(shader);
            // TODO: Implement a UBO for lights insteaad of this
            uploadLightsToShader(shader, lightData);

            m_CurrentShader = &shader;
        }

        draw(cmd);
    }

    // TODO:
    // Order Independent Transparency

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);

    m_RenderQueue.sortTransparent();
    for (const auto& cmd : m_RenderQueue.getTransparent()) {
        draw(cmd);
    }

    // -------------------- //

    glBindVertexArray(m_LineVAO);
    for (const auto& cmd : m_LineQueue) {
        glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            cmd.points.size() * sizeof(glm::vec3),
            cmd.points.data(),
            GL_DYNAMIC_DRAW);

        // drawArrays(GL_LINE_STRIP, cmd.points.size());
        drawArrays(GL_LINES, cmd.points.size());
    }
}

void OpenGLRenderer::drawLine(const std::vector<glm::vec3>& pts) {
    m_LineQueue.emplace_back(LineStripCmd{pts});
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

void OpenGLRenderer::draw(const RenderCommand& cmd) {
    if (!cmd.mesh) {
        ZK_CORE_ERROR("No Mesh for render command");
        return;
    }

    if (cmd.mesh->getIndexBuffer().getCount() == 0) {
        ZK_CORE_ERROR("Mesh has zero indices");
        return;
    }

    if (!cmd.material) {
        ZK_CORE_ERROR("No Material for render command");
        return;
    }

    cmd.material->setTextures(m_EnableTextures);
    cmd.material->setShininess(m_Shininess);

    cmd.material->bind();
    cmd.mesh->bind();

    Shader& shader = cmd.material->getShader();

    shader.setMat4("u_Model", cmd.modelMatrix);
    shader.setInt("u_Blinn", m_EnableBlinn);
    shader.setFloat("time", glfwGetTime());

    if (cmd.mesh->isTriangle()) {
        drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }
    else {
        drawElements(GL_LINES, cmd.mesh->getIndexBuffer().getCount());
    }
}

void OpenGLRenderer::bindGlobalUniforms(Shader& shader) {
    // Bind scene texture
    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, m_ShadowMapFBO->getDepthAttachment());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_DepthCubeMapFBO->getDepthAttachment());

    shader.setInt("u_ShadowCubeMap", 4);
    shader.setVec3("u_LightPos", m_LightPos);
    shader.setFloat("u_FarPlane", m_FarPlane);

    // Bind shadow map
    shader.setInt("u_ShadowMap", 3);

    shader.setMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
    shader.setVec3("u_LightDir", m_LightDir);
}

void OpenGLRenderer::updateCamera(Camera3D& camera) {
    CameraUBO data;
    data.view = camera.getView();
    data.proj = camera.getProjection();
    data.viewPos = glm::vec4(camera.getPosition(), 1.0f);
    m_CameraUBO->setData(&data, sizeof(CameraUBO));
}

LightData OpenGLRenderer::extractLights(const Scene& scene) {
    LightData lightData;

    auto& registry = scene.getRegistry();

    auto& pointLights = registry.getStorage<PointLight>();
    for (auto& [entity, pointLight] : pointLights) {
        auto& transform = registry.getComponent<Transform3D>(entity);

        lightData.pointLights.emplace_back(pointLight, transform.position);
    }

    auto& spotLights = registry.getStorage<SpotLight>();
    for (auto& [entity, spotLight] : spotLights) {
        auto& transform = registry.getComponent<Transform3D>(entity);

        lightData.spotLights.emplace_back(spotLight, transform.position);
    }

    auto& dirLights = registry.getStorage<DirectionalLight>();
    for (auto& [entity, dirLight] : dirLights) {
        lightData.dirLights.emplace_back(dirLight);
    }

    return lightData;
}

void OpenGLRenderer::uploadLightsToShader(Shader& shader, const LightData& data) {
    shader.setInt("u_PointLightCount", data.pointLights.size());
    for (size_t i = 0; i < data.pointLights.size(); i++) {
        shader.setLight(
            "u_PointLights[" + std::to_string(i) + "]",
            data.pointLights[i].light,
            data.pointLights[i].position);
    }

    for (size_t i = 0; i < data.spotLights.size(); i++) {
        shader.setLight(
            // "u_SpotLights[" + std::to_string(i) + "]",
            "u_SpotLight",
            data.spotLights[i].light,
            data.spotLights[i].position);
    }

    for (size_t i = 0; i < data.dirLights.size(); i++) {
        shader.setLight(
            // "u_DirLights[" + std::to_string(i) + "]", data.DirectionalLights[i]);
            "u_DirLights",
            data.dirLights[i]);
    }
}

void OpenGLRenderer::debug() {
    ImGui::Text("Draw Calls: %d", m_Stats.drawCalls);
    ImGui::Text("Triangle Count: %d", m_Stats.triangleCount);
    ImGui::Text("Vertex Count: %d", m_Stats.vertexCount);

    ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);

    ImGui::Combo(
        "Render Mode", reinterpret_cast<int*>(&m_RenderMode), "Forward\0Deferred\0Wireframe\0\0");
    ImGui::Checkbox("Enable Textures", &m_EnableTextures);
    ImGui::Checkbox("Enable Normals", &m_DebugNormalsEnabled);
    ImGui::Checkbox("Anti Aliasing", &m_AntiAliasing);

    ImGui::SliderFloat("Shininess", &m_Shininess, 0.0f, 360.0f);
    ImGui::Checkbox("Enable Blinn", &m_EnableBlinn);

    m_PostProcessor.debug();

    ImGui::SliderFloat3("LightDir", &m_LightDir.x, -1.0f, 1.0f);
    ImGui::SliderFloat3("LightPos", &m_LightPos.x, -100.0f, 100.0f);

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

    ImGui::Begin("Shadow Map");

    uint32_t shadowTex = m_ShadowMapFBO->getDepthAttachment();
    // uint32_t shadowTex = m_DepthCubeMapFBO->getDepthAttachment(); // Cant do this, its a cubemap

    ImGui::Image(
        (ImTextureID)(uintptr_t)shadowTex,
        ImVec2(512, 512),
        ImVec2(0, 1), // flip vertically (OpenGL)
        ImVec2(1, 0));

    ImGui::End();
}

void OpenGLRenderer::skyboxPass(Camera3D& camera) {
    if (!m_CubemapTexture)
        return;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glm::mat4 view = camera.getView();
    glm::mat4 proj = camera.getProjection();
    view = glm::mat4(glm::mat3(view));

    m_SkyboxShader->bind();
    m_SkyboxShader->setMat4("u_InvProj", glm::inverse(proj));
    m_SkyboxShader->setMat4("u_InvView", glm::inverse(view));

    m_SkyboxShader->setInt("u_Skybox", 0);
    m_CubemapTexture->bind();

    glBindVertexArray(m_EmptyVAO);
    drawArrays(GL_TRIANGLES, 3);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void OpenGLRenderer::debugNormalsPass() {
    if (!m_DebugNormalsEnabled)
        return;

    // glDepthMask(GL_FALSE);
    m_DebugNormals->bind();

    for (const auto& cmd : m_RenderQueue.getOpaque()) {
        cmd.mesh->bind();

        m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);

        drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    for (const auto& cmd : m_RenderQueue.getTransparent()) {
        cmd.mesh->bind();

        m_DebugNormals->setMat4("u_Model", cmd.modelMatrix);

        drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    // glDepthMask(GL_TRUE);
}

void OpenGLRenderer::clearQueues() {
    // Renderer queues
    m_RenderQueue.clear();

    m_LineQueue.clear();
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

void OpenGLRenderer::updateShadowMap() {
    // glCullFace(GL_FRONT);

    float near_plane = 1.0f, far_plane = 100.5f;
    // glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    //
    // glm::mat4 lightView =
    //     glm::lookAt(m_LookAt1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float shadowDistance = 22.0f;
    glm::vec3 lightPos = -m_LightDir * shadowDistance;

    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    // This fixes it?
    // glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, 30.0f, -30.0f, 1.0f, 60.0f);
    glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 60.0f);

    m_LightSpaceMatrix = lightProjection * lightView;

    m_ShadowShader->bind();
    m_ShadowShader->setMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);

    m_ShadowMapFBO->bind();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& cmd : m_RenderQueue.getOpaque()) {
        cmd.mesh->bind();

        m_ShadowShader->setMat4("u_Model", cmd.modelMatrix);

        drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    m_ShadowMapFBO->unbind();

    glDisable(GL_DEPTH_TEST);
    // glCullFace(GL_BACK);
}

void OpenGLRenderer::updateShadowCubeMap() {
    float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float near = 1.0f;
    m_FarPlane = 100.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, m_FarPlane);

    // Create 6 view matrices for each face of the cubemap
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(
        shadowProj *
        glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    // Render to cubemap
    // Cubemap
    m_DepthCubeMapFBO->bind();
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_DepthCubeMapShader->bind();

    for (size_t i = 0; i < shadowTransforms.size(); i++) {
        m_DepthCubeMapShader->setMat4(
            "u_ShadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    }

    m_DepthCubeMapShader->setVec3("u_LightPos", m_LightPos);
    m_DepthCubeMapShader->setFloat("u_FarPlane", m_FarPlane);

    for (const auto& cmd : m_RenderQueue.getOpaque()) {
        cmd.mesh->bind();

        m_DepthCubeMapShader->setMat4("u_Model", cmd.modelMatrix);

        drawElements(GL_TRIANGLES, cmd.mesh->getIndexBuffer().getCount());
    }

    m_DepthCubeMapFBO->unbind();

    // glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderer::render(const Scene& scene, const RenderView& view, const LightData& lightData) {
    view.framebuffer->bind();
    glEnable(GL_DEPTH_TEST);

    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);

    setViewport(view.viewport);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCamera(*view.camera);

    skyboxPass(*view.camera);
    geometryPass(lightData);
    debugNormalsPass();

    view.framebuffer->unbind();
}

} // namespace zk
