// #include "zkpch.hpp"
//
// #include "OpenGLRenderer.hpp"
//
// #include "ZkEngine/Utils/MeshFactory.hpp"
// #include "ZkEngine/Core/AppAcess.hpp"
// #include "ZkEngine/Renderer/Shader.hpp"
//
// #include "ZkEngine/Utils/Profiler.hpp"
// #include "ZkEngine/World/World.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Debug/OpenGLDebug.hpp"
//
// #include "ZkEngine/Renderer/API/OpenGL/Passes/ShadowPass.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Passes/DebugPass.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Passes/GeometryPass.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Passes/SkyboxPass.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Passes/BloomPass.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/Passes/PostProcessPass.hpp"
//
// #include "ZkEngine/Renderer/Framebuffer.hpp"
// #include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"
// #include "ZkEngine/Renderer/CameraUBO.hpp"
//
// #include "ZkEngine/Renderer/BatchRenderer.hpp"
// #include "ZkEngine/Renderer/Core/RenderContext.hpp"
// #include "imgui.h"
//
// namespace zk {
//
// // NOTE:
// // Make a UBO for this
// // - bind 1: Lights
// // - bind 2: Materials
//
// /*
//    TODO:
//    - Sort Render commands by shader, material, mesh
//    - Do Instance rendering
//    - Do Batch rendering
//    - Implement Lights UBO
//    - Implement Frustum & Occlusion Culling
//    - Implement outline stencil
//    */
// OpenGLRenderer::OpenGLRenderer() : RendererBackend(RenderAPI::OpenGL) {}
//
// OpenGLRenderer::~OpenGLRenderer() {}
//
// void OpenGLRenderer::init() {
//     TracyGpuContext;
//
//     OpenGLDebug::init(m_Debug);
//
//     m_ShadowPass = createScope<ShadowPass>(this);
//
//     m_SkyboxPass = createScope<SkyboxPass>(this);
//     m_GeometryPass = createScope<GeometryPass>(this);
//     m_DebugPass = createScope<DebugPass>(this);
//
//     m_BloomPass = createScope<BloomPass>(this);
//
//     m_PostProcessPass = createScope<PostProcessPass>();
//
//     // Blending
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//     // Culling
//     glEnable(GL_CULL_FACE);
//     glCullFace(GL_BACK);
//     glFrontFace(GL_CCW);
//
//     // 3D
//     glEnable(GL_DEPTH_TEST);
//
//     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//     // Seamless
//     glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//
//     // Anti Aliasing
//     glEnable(GL_MULTISAMPLE);
//
//     m_CameraUBO = createScope<UniformBuffer>(sizeof(CameraUBO), 0);
//     m_LightsUBO = createScope<UniformBuffer>(sizeof(LightsUBO), 1);
//
//     // Frame buffers
//     FramebufferSpec specs;
//     specs.width = 1920;
//     specs.height = 1080;
//     specs.attachments = {
//         // {TextureFormat::RGBA8, TextureType::Texture2D},
//         {TextureFormat::RGBA16F, TextureType::Texture2D}, // HDR Color Buffer
//         {TextureFormat::RGBA16F, TextureType::Texture2D}, // Bloom Buffer
//         {TextureFormat::Depth, TextureType::Texture2D},
//     };
//     m_WorldFBO = m_Device.createFramebuffer(specs);
//
//     FramebufferSpec editorSpecs;
//     editorSpecs.width = 512;
//     editorSpecs.height = 512;
//     editorSpecs.attachments = {
//         {TextureFormat::RGBA8, TextureType::Texture2D},
//         {TextureFormat::Depth, TextureType::Texture2D},
//     };
//
//     m_EditorFBO = m_Device.createFramebuffer(editorSpecs);
//     m_EditorPostFBO = m_Device.createFramebuffer(editorSpecs);
// }
//
// void OpenGLRenderer::update() {
//     m_Stats = {};
//
//     if (m_AntiAliasing)
//         glEnable(GL_MULTISAMPLE);
//     else
//         glDisable(GL_MULTISAMPLE);
//
//     switch (m_RenderMode) {
//     case RenderMode::Forward:
//         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//         break;
//
//     case RenderMode::Deferred:
//         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//         break;
//
//     case RenderMode::Wireframe:
//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//         break;
//
//     default:
//         break;
//     }
// }
//
// void OpenGLRenderer::renderWorld(World& world) {
//     ZK_PROFILE_SCOPE("World Render");
//
//     switch (m_RenderMode) {
//     case RenderMode::Forward:
//         renderForward(world);
//         break;
//     case RenderMode::Deferred:
//         break;
//     case RenderMode::Wireframe:
//         break;
//     }
// }
//
// void OpenGLRenderer::renderForward(World& world) {
//     LightData lightData = world.getLights();
//
//     updateLights(lightData);
//
//     RenderContext ctx;
//     ctx.world = &world;
//     ctx.queue = &m_RenderQueue;
//     ctx.lights = &lightData;
//
//     {
//         ZK_PROFILE_SCOPE("Shadow Pass");
//         m_ShadowPass->execute(ctx, {});
//     }
//
//     auto views = world.getRenderViews(*this);
//     for (auto& view : views) {
//         if (view.type == ViewType::Editor && !m_ActiveEditor) {
//             continue;
//         }
//         render(ctx, view);
//     }
// }
//
// void OpenGLRenderer::render(RenderContext& ctx, const RenderView& view) {
//     view.framebuffer->bind();
//     glEnable(GL_DEPTH_TEST);
//
//     glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
//
//     setViewport(view.viewport);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//     updateCamera(view.camera);
//
//     {
//         ZK_PROFILE_SCOPE("Skybox Pass");
//         m_SkyboxPass->execute(ctx, view);
//     }
//     m_DebugPass->execute(ctx, view);
//     {
//         ZK_PROFILE_SCOPE("Geometry Pass");
//         m_GeometryPass->execute(ctx, view);
//     }
//
//     view.framebuffer->unbind();
//
//     ctx.sceneColor = view.framebuffer->getColorAttachment(0);
//
//     if (view.type == ViewType::World) {
//         ctx.brightColor = view.framebuffer->getColorAttachment(1);
//         ctx.sceneDepth = view.framebuffer->getDepthAttachment();
//
//         {
//             ZK_PROFILE_SCOPE("Bloom Pass");
//             m_BloomPass->execute(ctx, view);
//         }
//         ctx.bloomBlur = m_BloomPass->getOutputTexture();
//
//         m_PostProcessPass->setOutput(nullptr);
//     }
//     else if (view.type == ViewType::Editor) {
//         m_PostProcessPass->setOutput(m_EditorPostFBO.get());
//     }
//
//     {
//         ZK_PROFILE_SCOPE("Post Process Pass");
//         m_PostProcessPass->execute(ctx, view);
//     }
// }
//
// void OpenGLRenderer::drawLine(const std::vector<glm::vec3>& pts) {
//     m_DebugPass->drawLine(pts);
// }
//
// void OpenGLRenderer::setClearColor(const glm::vec4& color) {
//     m_ClearColor[0] = color.r;
//     m_ClearColor[1] = color.g;
//     m_ClearColor[2] = color.b;
//     m_ClearColor[3] = color.a;
//
//     glClearColor(color.r, color.g, color.b, color.a);
// }
//
// void OpenGLRenderer::resize(uint32_t width, uint32_t height) {
//     m_WorldFBO->resize(width, height);
//     // glViewport(0, 0, width, height);
// }
//
// void OpenGLRenderer::setViewport(const Viewport& viewport) {
//     glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
// }
//
// void OpenGLRenderer::updateCamera(const CameraData& camera) {
//     CameraUBO data;
//     data.view = camera.view;
//     data.proj = camera.proj;
//     data.viewPos = glm::vec4(camera.position, 1.0f);
//     data.nearPlane = camera.nearPlane;
//     data.farPlane = camera.farPlane;
//
//     m_CameraUBO->setData(&data, sizeof(CameraUBO));
// }
//
// void OpenGLRenderer::updateLights(const LightData& data) {
//     LightsUBO ubo;
//
//     constexpr int MAX_LIGHTS = 4;
//
//     size_t dirCount = std::min(data.dirLights.size(), (size_t)MAX_LIGHTS);
//     size_t pointCount = std::min(data.pointLights.size(), (size_t)MAX_LIGHTS);
//     size_t spotCount = std::min(data.spotLights.size(), (size_t)MAX_LIGHTS);
//
//     ubo.dirLightCount = dirCount;
//     ubo.pointLightCount = pointCount;
//     ubo.spotLightCount = spotCount;
//
//     // Point Lights
//     for (size_t i = 0; i < pointCount; i++) {
//         auto& src = data.pointLights[i];
//         auto& dst = ubo.pointLights[i];
//
//         glm::vec3 diffuse = src.light.color * src.light.diffuse;
//         glm::vec3 ambient = diffuse * src.light.ambient;
//         glm::vec3 specular = src.light.color * src.light.specular;
//
//         dst.position = glm::vec4(src.position, 1.0f);
//         dst.intensity = glm::vec4(src.light.intensity);
//
//         dst.ambient = glm::vec4(ambient, 1.0f);
//         dst.diffuse = glm::vec4(diffuse, 1.0f);
//         dst.specular = glm::vec4(specular, 1.0f);
//
//         dst.attenuation =
//             glm::vec4(src.light.constant, src.light.linear, src.light.quadratic, src.farPlane);
//     }
//
//     // Dir Lights
//     for (size_t i = 0; i < dirCount; i++) {
//         auto& src = data.dirLights[i];
//         auto& dst = ubo.dirLights[i];
//
//         glm::vec3 diffuse = src.light.diffuse * src.light.color;
//         glm::vec3 ambient = diffuse * src.light.ambient;
//         glm::vec3 specular = src.light.specular * src.light.color;
//
//         dst.direction = glm::vec4(src.light.direction, 0.0f);
//         dst.intensity = glm::vec4(src.light.intensity);
//
//         dst.ambient = glm::vec4(ambient, 1.0f);
//         dst.diffuse = glm::vec4(diffuse, 1.0f);
//         dst.specular = glm::vec4(specular, 1.0f);
//     }
//
//     // Spot Lights
//     for (size_t i = 0; i < spotCount; i++) {
//         auto& src = data.spotLights[i];
//         auto& dst = ubo.spotLights[i];
//
//         glm::vec3 diffuse = src.light.color * src.light.diffuse;
//         glm::vec3 ambient = diffuse * src.light.ambient;
//         glm::vec3 specular = src.light.color * src.light.specular;
//
//         dst.position = glm::vec4(src.position, 1.0f);
//         dst.direction = glm::vec4(src.light.direction, 0.0f);
//         dst.intensity = glm::vec4(src.light.intensity);
//
//         dst.ambient = glm::vec4(ambient, 1.0f);
//         dst.diffuse = glm::vec4(diffuse, 1.0f);
//         dst.specular = glm::vec4(specular, 1.0f);
//
//         dst.attenuation =
//             glm::vec4(src.light.constant, src.light.linear, src.light.quadratic, 0.0f);
//     }
//
//     m_LightsUBO->setData(&ubo, sizeof(LightsUBO));
// }
//
// void OpenGLRenderer::debug() {
//     ImGui::Text("Draw Calls: %d", m_Stats.drawCalls);
//     ImGui::Text("Triangle Count: %d", m_Stats.triangleCount);
//     ImGui::Text("Vertex Count: %d", m_Stats.indexCount);
//
//     ImGui::ColorEdit4("Clear Color", &m_ClearColor[0]);
//
//     ImGui::Combo(
//         "Render Mode", reinterpret_cast<int*>(&m_RenderMode), "Forward\0Deferred\0Wireframe\0\0");
//     ImGui::Checkbox("Anti Aliasing", &m_AntiAliasing);
//
//     if (ImGui::TreeNode("Shadow Pass")) {
//         m_ShadowPass->debug();
//         ImGui::TreePop();
//     }
//
//     if (ImGui::TreeNode("Debug Pass")) {
//         m_DebugPass->debug();
//         ImGui::TreePop();
//     }
//
//     if (ImGui::TreeNode("Geometry Pass")) {
//         m_GeometryPass->debug();
//         ImGui::TreePop();
//     }
//
//     if (ImGui::TreeNode("Bloom Pass")) {
//         m_BloomPass->debug();
//         ImGui::TreePop();
//     }
//
//     m_PostProcessPass->debug();
//     // m_EditorCamera->debug();
//
//     if (ImGui::Begin("World View")) {
//         m_ActiveEditor = true;
//
//         ImVec2 size = ImGui::GetContentRegionAvail();
//         static ImVec2 lastSize = {0, 0};
//
//         if (size.x > 0 && size.y > 0 && (size.x != lastSize.x || size.y != lastSize.y)) {
//             m_EditorPostFBO->resize((uint32_t)size.x, (uint32_t)size.y);
//             lastSize = size;
//         }
//
//         uint32_t tex = m_EditorPostFBO->getColorAttachment(0);
//
//         ImGui::Image(
//             (ImTextureID)(uintptr_t)tex,
//             ImVec2(m_EditorPostFBO->getWidth(), m_EditorPostFBO->getHeight()),
//             ImVec2(0, 1),
//             ImVec2(1, 0));
//     }
//     else {
//         m_ActiveEditor = false;
//     }
//     ImGui::End();
// }
//
// void OpenGLRenderer::setSkybox(const Ref<Texture>& tex) {
//     m_SkyboxPass->setSkybox(tex);
// }
//
// void OpenGLRenderer::drawArrays(uint32_t mode, uint32_t count) {
//     if (count <= 0) {
//         ZK_CORE_ERROR("Invalid index count: {}", count);
//         return;
//     }
//
//     glDrawArrays(mode, 0, count);
//     m_Stats.drawCalls++;
//     if (mode == GL_TRIANGLES)
//         m_Stats.triangleCount += count / 3;
//     m_Stats.indexCount += count;
//
//     ZK_CORE_DEBUG("CALLED ARRAYS");
// }
//
// void OpenGLRenderer::drawElements(uint32_t mode, uint32_t indexCount) {
//     if (indexCount <= 0) {
//         ZK_CORE_ERROR("Invalid index count: {}", indexCount);
//         return;
//     }
//
//     glDrawElements(mode, indexCount, GL_UNSIGNED_INT, nullptr);
//     m_Stats.drawCalls++;
//     if (mode == GL_TRIANGLES)
//         m_Stats.triangleCount += indexCount / 3;
//     m_Stats.indexCount += indexCount;
//
//     ZK_CORE_DEBUG("CALLED ELEMENTS");
// }
//
// void OpenGLRenderer::drawMesh(const Ref<Mesh>& mesh) {
//     if (!mesh) {
//         ZK_CORE_ERROR("Invalid mesh");
//         return;
//     }
//
//     uint32_t indexCount = mesh->getIndexBuffer().getCount();
//
//     if (indexCount <= 0) {
//         ZK_CORE_ERROR("Invalid index count: {}", indexCount);
//         return;
//     }
//
//     mesh->bind();
//     glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
//     // glMultiDrawElementsIndirect()
//     // glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, 20);
//
//     m_Stats.drawCalls++;
//     m_Stats.triangleCount += indexCount / 3;
//     m_Stats.indexCount += indexCount;
//
//     // ZK_CORE_DEBUG("CALLED MESH");
// }
//
// void OpenGLRenderer::drawInstanced(int count, int instanceCount) {
//     glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount);
//     m_Stats.drawCalls++;
//     m_Stats.triangleCount += count / 3;
//     m_Stats.indexCount += count * instanceCount;
//
//     // ZK_CORE_DEBUG("CALLED INSTANCED");
// }
//
// void OpenGLRenderer::drawBatch(const Batch& batch) {
//     int count = batch.key.mesh->getIndexBuffer().getCount();
//
//     glDrawElementsInstancedBaseInstance(
//         GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, batch.instanceCount, batch.instanceOffset);
//
//     m_Stats.drawCalls++;
//     uint32_t trianglesPerInstance = count / 3;
//     m_Stats.triangleCount += trianglesPerInstance * batch.instanceCount;
//     m_Stats.indexCount += count * batch.instanceCount;
// }
//
// void OpenGLRenderer::renderScreenQuad() {
//     if (m_QuadVAO == 0) {
//         float quadVertices[] = {                          // positions   // texCoords
//                                 -1.0f, 1.0f,  0.0f, 1.0f, //
//                                 -1.0f, -1.0f, 0.0f, 0.0f, //
//                                 1.0f,  -1.0f, 1.0f, 0.0f, //
//
//                                 -1.0f, 1.0f,  0.0f, 1.0f, //
//                                 1.0f,  -1.0f, 1.0f, 0.0f, //
//                                 1.0f,  1.0f,  1.0f, 1.0f};
//
//         glGenVertexArrays(1, &m_QuadVAO);
//         glGenBuffers(1, &m_QuadVBO);
//
//         glBindVertexArray(m_QuadVAO);
//         glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//
//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(
//             1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//     }
//
//     glBindVertexArray(m_QuadVAO);
//     glDrawArrays(GL_TRIANGLES, 0, 6);
// }
//
// } // namespace zk
