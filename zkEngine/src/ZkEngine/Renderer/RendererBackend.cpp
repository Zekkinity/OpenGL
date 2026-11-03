#include "ZkEngine/Renderer/Core/FrameData.hpp"
#include "zkpch.hpp"

#include "RendererBackend.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLDevice.hpp"
#include "ZkEngine/Renderer/API/Vulkan/VulkanDevice.hpp"

#include "ZkEngine/Renderer/DeferredRenderer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/UniformBuffer.hpp"
#include "ZkEngine/Renderer/CameraUBO.hpp"

#include "ZkEngine/Renderer/API/OpenGL/InstanceBuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

#include "ZkEngine/World/Entity.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "imgui.h"

namespace zk {

// enum RenderFlags : uint32_t {
//     None = 0,
//     Selected = 1 << 0,
//     Transparent = 1 << 1,
//     CastShadow = 1 << 2,
//     EditorOnly = 1 << 3
// };

RendererBackend::RendererBackend(RenderAPI api) : m_API(api) {}

RendererBackend::~RendererBackend() {}

Scope<RendererBackend> RendererBackend::create(RenderAPI api) {
    Scope<RendererBackend> backend = createScope<RendererBackend>(api);

    switch (api) {
    case RenderAPI::OpenGL:
        backend->m_Device = createScope<OpenGLDevice>();
        break;
    case RenderAPI::Vulkan:
        backend->m_Device = createScope<VulkanDevice>();
        break;
    }

    return backend;
}

void RendererBackend::init() {
    Profiler::init();

    m_Device->init();

    m_BatchRenderer = createScope<BatchRenderer>(*m_Device);

    if (m_API == RenderAPI::OpenGL)
        ZK_CORE_DEBUG("Using Deferred Shading | OpenGL");

    m_Pipeline = createScope<DeferredRenderer>();
    m_Pipeline->init(*m_Device);

    // UBOs
    m_CameraUBO = createScope<UniformBuffer>(sizeof(CameraUBO), 0);
    m_LightsUBO = createScope<UniformBuffer>(sizeof(LightsUBO), 1);

    m_ShadowPass = createScope<ShadowPass>(m_Device.get());

    // Post process
    m_PostProcessShader = Asset::loadShader("shaders/Screen.vert", "shaders/Screen.frag");

    m_BloomPass = createScope<BloomPass>(m_Device.get());
}

void RendererBackend::beginFrame(const World& world) {
    // Fill Render Queue
    getSceneData(world);
    m_RenderQueue.sortOpaque();

    m_BatchRenderer->build(m_RenderQueue);
    m_BatchRenderer->updateInstances();

    // auto views = world.getRenderViews(*this);
    // updateCamera(views[0].camera);
}

void RendererBackend::render(World& world) {
    FrameData frameData{
        .lights = world.getLights(),
        .camera = world.getCameraData(world.getActiveCamera()),
    };

    updateCamera(frameData.camera);
    updateLights(frameData.lights);

    // Shadow Pass
    {
        m_ShadowPass->execute(frameData, m_RenderQueue);
    }

    uint32_t sceneColor = m_Pipeline->render(frameData, *m_BatchRenderer);

    {
        // ZK_PROFILE_SCOPE("Forward Shading Pass");
        // renderForward();
    }

    uint32_t bloomTexture = 0;

    if (m_Bloom)
        bloomTexture = m_BloomPass->execute(sceneColor);

    // m_PostProcessPass->execute(sceneColor)

    postProcessPass(sceneColor, bloomTexture);

    // Maybe a present call that just draws to the default fbo
}

// void RendererBackend::renderForward() {
//     Framebuffer* gBuffer = m_Pipeline->getGBuffer();
//
//     glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->getID());
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//     glBlitFramebuffer(
//         0,
//         0,
//         m_GBuffer->getWidth(),
//         m_GBuffer->getHeight(),
//         0,
//         0,
//         m_GBuffer->getWidth(),
//         m_GBuffer->getHeight(),
//         GL_DEPTH_BUFFER_BIT,
//         GL_NEAREST);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//     m_LightingFBO->bind();
//
//     // Forward Shading
//     glEnable(GL_DEPTH_TEST);
//     glDepthFunc(GL_LESS); // or GL_LEQUAL
//
//     m_LightSource->bind();
//
//     // Draw Lights in Forward Shading
//     for (size_t i = 0; i < lights.pointLights.size(); i++) {
//         const auto& light = lights.pointLights[i];
//
//         glm::mat4 model = glm::mat4(1.0f);
//         model = glm::translate(model, light.position);
//         model = glm::scale(model, glm::vec3(0.25f));
//
//         m_LightSource->setMat4("u_Model", model);
//         m_LightSource->setVec4(
//             "u_Color", glm::vec4(light.light.color * light.light.intensity, 1.0));
//
//         m_Device->drawMesh(m_CubeMesh);
//     }
//
//     m_LightSource->unbind();
// }

void RendererBackend::postProcessPass(uint32_t sceneColor, uint32_t bloomTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    m_PostProcessShader->bind();

    // Bind scene texture
    glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_InputTexture);
    glBindTexture(GL_TEXTURE_2D, sceneColor);

    // // Bind bloom texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    // glBindTexture(GL_TEXTURE_2D, ctx.bloomBlur);
    //
    // // Bind depth texture
    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, ctx.sceneDepth);

    m_PostProcessShader->setInt("u_ScreenTexture", 0);
    m_PostProcessShader->setInt("u_BloomTexture", 1);
    m_PostProcessShader->setInt("u_ScreenDepth", 2);

    m_PostProcessShader->setInt("u_GammaCorrection", m_GammaCorrection);
    m_PostProcessShader->setInt("u_Kernel", m_Kernel);
    m_PostProcessShader->setInt("u_HDR", m_HDR);
    m_PostProcessShader->setFloat("u_Exposure", m_Exposure);
    m_PostProcessShader->setFloat("u_BloomStrength", m_BloomStrength);

    m_Device->drawScreenQuad();
}

void RendererBackend::endFrame() {
    m_RenderQueue.clear();
}

void RendererBackend::debug() {
    m_ShadowPass->debug();
    m_Pipeline->debug();

    ImGui::Checkbox("Gamma Correction", &m_GammaCorrection);
    ImGui::Checkbox("Kernel", &m_Kernel);
    ImGui::Checkbox("HDR", &m_HDR);
    ImGui::SliderFloat("Exposure", &m_Exposure, 0.0f, 10.0f);

    ImGui::Checkbox("Bloom", &m_Bloom);
    ImGui::SliderFloat("Bloom Strength", &m_BloomStrength, 0.0f, 0.5f);

    m_BloomPass->debug();
}

void RendererBackend::resize(uint32_t width, uint32_t height) {
    m_Pipeline->resize(width, height);
}

void RendererBackend::updateCamera(const CameraData& camera) {
    CameraUBO data;
    data.view = camera.view;
    data.proj = camera.proj;
    data.viewPos = glm::vec4(camera.position, 1.0f);
    data.nearPlane = camera.nearPlane;
    data.farPlane = camera.farPlane;

    m_CameraUBO->setData(&data, sizeof(CameraUBO));
}

void RendererBackend::updateLights(const LightData& data) {
    LightsUBO ubo;

    constexpr int MAX_LIGHTS = 4;

    size_t dirCount = std::min(data.dirLights.size(), (size_t)MAX_LIGHTS);
    size_t pointCount = std::min(data.pointLights.size(), (size_t)MAX_POINT_LIGHTS);
    size_t spotCount = std::min(data.spotLights.size(), (size_t)MAX_LIGHTS);

    ubo.dirLightCount = dirCount;
    ubo.pointLightCount = pointCount;
    ubo.spotLightCount = spotCount;

    // Point Lights
    for (size_t i = 0; i < pointCount; i++) {
        auto& src = data.pointLights[i];
        auto& dst = ubo.pointLights[i];

        glm::vec3 diffuse = src.light.color * src.light.diffuse;
        glm::vec3 ambient = diffuse * src.light.ambient;
        glm::vec3 specular = src.light.color * src.light.specular;

        dst.position = glm::vec4(src.position, 1.0f);
        // dst.intensity = src.light.intensity;
        dst.color = glm::vec4(src.light.color * src.light.intensity, 1.0f);

        dst.ambient = glm::vec4(ambient, 1.0f);
        dst.diffuse = glm::vec4(diffuse, 1.0f);
        dst.specular = glm::vec4(specular, 1.0f);

        float lightMax =
            std::fmaxf(std::fmaxf(src.light.color.r, src.light.color.g), src.light.color.b);
        float radius =
            (-src.light.linear +
             std::sqrtf(
                 src.light.linear * src.light.linear -
                 4 * src.light.quadratic * (src.light.constant - (256.0 / 5.0) * lightMax))) /
            (2 * src.light.quadratic);

        dst.attenuation =
            glm::vec4(src.light.constant, src.light.linear, src.light.quadratic, src.farPlane);
        // dst.radius = radius;
    }

    // Dir Lights
    for (size_t i = 0; i < dirCount; i++) {
        auto& src = data.dirLights[i];
        auto& dst = ubo.dirLights[i];

        glm::vec3 diffuse = src.light.diffuse * src.light.color;
        glm::vec3 ambient = diffuse * src.light.ambient;
        glm::vec3 specular = src.light.specular * src.light.color;

        dst.direction = glm::vec4(src.light.direction, 0.0f);
        dst.color = glm::vec4(src.light.color * src.light.intensity, 1.0f);
        // dst.intensity = glm::vec4(src.light.intensity);

        dst.ambient = glm::vec4(ambient, 1.0f);
        dst.diffuse = glm::vec4(diffuse, 1.0f);
        dst.specular = glm::vec4(specular, 1.0f);
    }

    // Spot Lights
    for (size_t i = 0; i < spotCount; i++) {
        auto& src = data.spotLights[i];
        auto& dst = ubo.spotLights[i];

        glm::vec3 diffuse = src.light.color * src.light.diffuse;
        glm::vec3 ambient = diffuse * src.light.ambient;
        glm::vec3 specular = src.light.color * src.light.specular;

        dst.position = glm::vec4(src.position, 1.0f);
        dst.direction = glm::vec4(src.light.direction, 0.0f);
        dst.cutOff = src.light.cutOff;
        dst.outerCutOff = src.light.outerCutOff;

        // dst.intensity = glm::vec4(src.light.intensity);
        dst.color = glm::vec4(src.light.color * src.light.intensity, 1.0f);

        dst.ambient = glm::vec4(ambient, 1.0f);
        dst.diffuse = glm::vec4(diffuse, 1.0f);
        dst.specular = glm::vec4(specular, 1.0f);

        dst.attenuation =
            glm::vec4(src.light.constant, src.light.linear, src.light.quadratic, 0.0f);
    }

    m_LightsUBO->setData(&ubo, sizeof(LightsUBO));
}

void RendererBackend::getSceneData(const World& world) {
    Entity activeCamera = world.getActiveCamera();
    CameraData camData = world.getCameraData(activeCamera);

    { // Render all renderable objects
        auto view = world.getRegistry().view<RenderObject, Transform3D>();
        for (auto [entity, renderObject, transform] : view.each()) {
            if (m_FrustumCulling) {
                AABB worldAABB = renderObject.mesh->getAABB().transform(transform.getMatrix());
                if (!isVisible(camData.frustum, worldAABB)) {
                    continue;
                }
            }

            RenderCommand cmd;
            cmd.mesh = renderObject.mesh;
            cmd.material = renderObject.material;
            cmd.modelMatrix = transform.getMatrix();
            cmd.distanceToCamera = glm::length2(camData.position - transform.position);

            // cmd.selected = world.getRegistry().all_of<Selected>(entity);

            m_RenderQueue.submit(cmd);
        }
    }

    { // Render all models
        auto view = world.getRegistry().view<ModelComponent, Transform3D>();
        for (auto [entity, model, transform] : view.each()) {
            glm::mat4 offset = glm::mat4(1.0f);

            // Camera model needs to be rotated to face where it is looking
            if (world.getRegistry().all_of<Camera3D>(entity)) {
                glm::vec3 modelForward = glm::vec3(0, -1, 0);
                glm::vec3 targetForward = glm::vec3(0, 0, -1);
                glm::quat q = glm::rotation(modelForward, targetForward);
                offset = glm::mat4_cast(q);

                offset = glm::rotate(offset, glm::radians(180.0f), glm::vec3(0, 1, 0));
            }

            // Frustum culling per model
            if (m_FrustumCulling) {
                AABB worldAABB = model.model->getBounds().transform(transform.getMatrix());
                if (!isVisible(camData.frustum, worldAABB)) {
                    continue;
                }
            }

            for (auto& mesh : model.model->getMeshes()) {
                RenderCommand cmd;

                cmd.mesh = mesh.mesh;
                cmd.material = mesh.material;
                cmd.modelMatrix = transform.getMatrix() * offset;
                cmd.distanceToCamera = glm::length2(camData.position - transform.position);

                m_RenderQueue.submit(cmd);
            }
        }
    }
}

// AABB
bool RendererBackend::isVisible(const Frustum& frustum, const AABB& box) const {
    for (int i = 0; i < 6; i++) {
        const auto& plane = frustum.planes[i];

        glm::vec3 p;

        p.x = (plane.normal.x >= 0.0f) ? box.max.x : box.min.x;
        p.y = (plane.normal.y >= 0.0f) ? box.max.y : box.min.y;
        p.z = (plane.normal.z >= 0.0f) ? box.max.z : box.min.z;

        if (plane.getDistanceToPoint(p) < 0.0f) {
            return false;
        }
    }

    return true;
}

} // namespace zk
