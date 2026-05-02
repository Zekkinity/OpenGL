#include "zkpch.hpp"

#include "World.hpp"

#include "ZkEngine/World/Entity.hpp"
#include "ZkEngine/World/PlayerController.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/Asset/AssetManager.hpp"
#include "ZkEngine/Renderer/Core/RenderView.hpp"
#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "imgui.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace zk {

World::World() {}
World::~World() {}

void World::onInit(AssetManager* assetManager) {
    m_PlayerController = createScope<PlayerController>();
    m_AssetManager = assetManager;

    Entity mainCamera = createCamera();
    Entity editorCamera = createCamera();
    // auto otherCamera = createCamera({CameraConfig({
    //     .aspect = 512.0f / 512.0f,
    // })});

    m_ActiveCamera = mainCamera.m_EntityHandle;
    m_EditorCamera = editorCamera.m_EntityHandle;

    setActiveCamera(mainCamera);

    init();
}

Entity World::createEntity() {
    Entity entity = {m_Registry.create(), this};
    entity.addComponent<Transform3D>();

    return entity;
}

void World::destroyEntity(Entity entity) {
    m_Registry.destroy(entity.m_EntityHandle);
}

void World::clear() {
    std::vector<EntityHandle> toDestroy;

    for (auto entity : m_Registry.view<RenderObject, Transform3D>()) {
        if (m_Registry.all_of<Camera3D>(entity))
            continue;

        toDestroy.push_back(entity);
    }

    for (auto& entity : toDestroy) {
        if (m_Registry.valid(entity)) {
            m_Registry.destroy(entity);
        }
    }
}

CameraData World::getCameraData(Entity camera) const {
    const auto& transform = camera.getComponent<Transform3D>();
    const auto& cameraComp = camera.getComponent<Camera3D>();

    CameraData data;
    data.view = transform.getView();
    data.proj = cameraComp.getProjection();
    data.position = glm::vec4(transform.position, 1.0f);
    data.frustum = getFrustum(cameraComp.getProjection(), transform.getView());
    data.nearPlane = cameraComp.getZNear();
    data.farPlane = cameraComp.getZFar();

    return data;
}

Frustum World::getFrustum(const glm::mat4& proj, const glm::mat4& view) const {
    Frustum frustum;

    glm::mat4 vp = proj * view;

    // Left
    frustum.planes[0].normal.x = vp[0][3] + vp[0][0];
    frustum.planes[0].normal.y = vp[1][3] + vp[1][0];
    frustum.planes[0].normal.z = vp[2][3] + vp[2][0];
    frustum.planes[0].distance = vp[3][3] + vp[3][0];

    // Right
    frustum.planes[1].normal.x = vp[0][3] - vp[0][0];
    frustum.planes[1].normal.y = vp[1][3] - vp[1][0];
    frustum.planes[1].normal.z = vp[2][3] - vp[2][0];
    frustum.planes[1].distance = vp[3][3] - vp[3][0];

    // Bottom
    frustum.planes[2].normal.x = vp[0][3] + vp[0][1];
    frustum.planes[2].normal.y = vp[1][3] + vp[1][1];
    frustum.planes[2].normal.z = vp[2][3] + vp[2][1];
    frustum.planes[2].distance = vp[3][3] + vp[3][1];

    // Top
    frustum.planes[3].normal.x = vp[0][3] - vp[0][1];
    frustum.planes[3].normal.y = vp[1][3] - vp[1][1];
    frustum.planes[3].normal.z = vp[2][3] - vp[2][1];
    frustum.planes[3].distance = vp[3][3] - vp[3][1];

    // Near
    frustum.planes[4].normal.x = vp[0][3] + vp[0][2];
    frustum.planes[4].normal.y = vp[1][3] + vp[1][2];
    frustum.planes[4].normal.z = vp[2][3] + vp[2][2];
    frustum.planes[4].distance = vp[3][3] + vp[3][2];

    // Far
    frustum.planes[5].normal.x = vp[0][3] - vp[0][2];
    frustum.planes[5].normal.y = vp[1][3] - vp[1][2];
    frustum.planes[5].normal.z = vp[2][3] - vp[2][2];
    frustum.planes[5].distance = vp[3][3] - vp[3][2];

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(frustum.planes[i].normal);
        frustum.planes[i].normal /= length;
        frustum.planes[i].distance /= length;
    }

    return frustum;
}

Entity World::createCamera(const CameraConfig& config) {
    auto camera = createEntity();
    camera.addCamera(config);

    // auto shader = m_AssetManager->loadShader("shaders/lighting.vert",
    // "shaders/lightSource.frag"); auto material = m_AssetManager->loadMaterial({shader}); auto
    // cameraMesh = m_AssetManager->getMesh(PrimitiveType::Cube);

    // auto shader =
    //     m_AssetManager->loadShader("shaders/lighting.vert", "shaders/multipleLightsShadow.frag");
    // auto canonModel = m_AssetManager->loadModel("models/canon/scene.gltf");
    // canonModel->rotate(180.0f, {0.0, 1.0, 0.0});
    //
    // camera.addModel(canonModel, shader);
    camera.setPosition({0.0f, 0.0f, -20.0f});
    camera.setScale({1.0f, 1.0f, 1.0f});
    // camera.setRotation({0.0f, 180.0f, 0.0f});

    // camera.addRenderable(cameraMesh, material);
    // camera.setScale({1.0f, 1.0f, 1.0f});

    return camera;
}

void World::onUpdate(float dt) {
    m_PlayerController->update(dt);

    // Manage all rigid bodies
    auto view = m_Registry.view<RigidBody, Transform3D>();
    for (auto [entity, rb, transform] : view.each()) {
        // Integrate velocity
        rb.velocity += rb.acceleration * dt;

        // Apply drag
        rb.velocity -= rb.velocity * rb.drag * dt;
        // rb.velocity *= 1.0f / (1.0f + rb.drag * dt);

        // Integrate position
        transform.position += rb.velocity * dt;

        // Reset acc
        rb.acceleration = glm::vec3(0.0f);
    }

    // Update point light color
    auto pointView = m_Registry.view<PointLight, RenderObject>();
    for (auto [entity, pointLight, renderObject] : pointView.each()) {
        renderObject.material->setColor({pointLight.color, 1.0f});
    }

    update(dt);
}

LightData World::getLights() const {
    LightData lightData;

    // Point Lights
    // auto group = m_Registry.group<PointLight>(entt::get<Transform3D>);
    auto pointLights = m_Registry.view<PointLight, Transform3D>();
    for (auto [entity, pointLight, transform] : pointLights.each()) {
        lightData.pointLights.emplace_back(pointLight, transform.position);
    }

    // Spot Lights
    auto spotLights = m_Registry.view<SpotLight, Transform3D>();
    for (auto [entity, spotlight, transform] : spotLights.each()) {
        lightData.spotLights.emplace_back(spotlight, transform.position);
    }

    // Directional Lights
    auto dirLights = m_Registry.view<DirectionalLight>();
    for (auto [entity, dirLight] : dirLights.each()) {
        lightData.dirLights.emplace_back(dirLight);
    }

    return lightData;
}

void World::onWindowResize(int width, int height) {
    auto view = m_Registry.view<Camera3D>();
    for (auto [entity, camera] : view.each()) {
        camera.onWindowResize(width, height);
    }

    // m_Registry.getComponent<Camera3D>(m_Cameras[m_ActiveCameraIndex]).onWindowResize(width,
    // height);
}

std::vector<RenderView> World::getRenderViews(RendererBackend& renderer) const {
    std::vector<RenderView> views;

    // --- Main World View ---
    {
        RenderView sceneView;
        // sceneView.framebuffer = renderer.getWorldFBO();
        sceneView.viewport = {
            0, 0, sceneView.framebuffer->getWidth(), sceneView.framebuffer->getHeight()};
        sceneView.camera = getCameraData(getActiveCamera());
        sceneView.type = ViewType::World;

        views.push_back(sceneView);
    }

    // --- Editor View ---
    {
        RenderView editorView;
        // editorView.framebuffer = renderer.getEditorFBO();
        editorView.viewport = {
            0, 0, editorView.framebuffer->getWidth(), editorView.framebuffer->getHeight()};
        editorView.camera = getCameraData(getEditorCamera());
        editorView.type = ViewType::Editor;

        views.push_back(editorView);
    }

    return views;
}

void World::onDebug() {
    // TODO: Check for maximum number of lights

    auto count = m_Registry.view<RenderObject, Transform3D>().size_hint();
    ImGui::Text("Render Objects: %zu", count);

    ImGui::Text("Light System");
    if (ImGui::TreeNode("PointLights")) {
        if (ImGui::Button("Add")) {
            auto spotlight = createEntity();
            auto mesh = m_AssetManager->getMesh(PrimitiveType::Cube);
            auto material = m_AssetManager->loadMaterial(
                {.shader = m_AssetManager->loadShader(
                     "shaders/lighting.vert", "shaders/lightSource.frag")});
            spotlight.addRenderable(mesh, material);
            // spotlight.setColor({5.0f, 0.0f, 0.0f, 1.0f});

            // spotlight.setPosition(getCamera()->getPosition());
            spotlight.setScale(glm::vec3(0.25f));
            spotlight.addPointLight({.color = glm::vec3(1.0f, 1.0f, 1.0f)});
        }

        auto view = m_Registry.view<PointLight, Transform3D>();

        ImGui::Text("Amount: %zu", view.size_hint());
        ImGui::PushID("PointLights");
        for (auto [entity, pointLight, transform] : view.each()) {
            ImGui::PushID((uint32_t)entity);

            ImGui::Text("ID: %u", entity);
            ImGui::SliderFloat3("Position", &transform.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("Color", &pointLight.color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Itensity", &pointLight.intensity, 0.0f, 100.0f);
            ImGui::SliderFloat("Linear", &pointLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &pointLight.quadratic, 0.0f, 0.5f);
            ImGui::SliderFloat("Ambient", &pointLight.ambient, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse", &pointLight.diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &pointLight.specular, 0.0f, 1.0f);

            if (ImGui::Button("Delete")) {
                m_Registry.destroy(entity);
            }

            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Directional Lights")) {
        if (ImGui::Button("Add")) {
            auto dirLight = createEntity();
            dirLight.addDirectionalLight({.direction = {-0.2f, -1.0f, -0.3f}});
        }

        auto view = m_Registry.view<DirectionalLight>();

        ImGui::Text("Amount: %zu", view.size());
        ImGui::PushID("DirectionalLights");
        for (auto [entity, dirLight] : view.each()) {
            ImGui::Separator();

            ImGui::PushID((uint32_t)entity);

            ImGui::Text("ID: %u", entity);
            ImGui::SliderFloat3("Direction", &dirLight.direction.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Itensity", &dirLight.intensity, 0.0f, 100.0f);
            ImGui::SliderFloat3("Color", &dirLight.color.x, 0.0f, 1.0f);

            if (ImGui::Button("Delete")) {
                m_Registry.destroy(entity);
            }

            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SpotLights")) {
        auto view = m_Registry.view<SpotLight, Transform3D>();

        ImGui::Text("Spot Lights: %zu", view.size_hint());
        ImGui::PushID("SpotLights");
        for (auto [entity, spotLight, transform] : view.each()) {
            ImGui::PushID((uint32_t)entity);

            ImGui::Text("Spot Light %u", entity);
            ImGui::SliderFloat3("Position", &transform.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("Direction", &spotLight.direction.x, -1.0f, 1.0f);
            ImGui::SliderFloat3("Color", &spotLight.color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Itensity", &spotLight.intensity, 0.0f, 100.0f);
            ImGui::SliderFloat("Inner", &spotLight.cutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Outer", &spotLight.outerCutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 0.5f);

            if (ImGui::Button("Delete")) {
                m_Registry.destroy(entity);
            }

            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    auto view = m_Registry.view<Camera3D, Transform3D>();

    ImGui::Text("Cameras: %zu", view.size_hint());

    static std::unordered_map<entt::entity, glm::vec2> angles;
    static std::unordered_set<entt::entity> initialized;

    int i = 0;
    for (auto [entity, camera, transform] : view.each()) {
        ImGui::PushID(i++);

        const bool isActive = (entity == m_ActiveCamera);
        const bool isEditor = (entity == m_EditorCamera);

        ImGui::Separator();

        // --- Header with state ---
        ImGui::Text(
            "Camera %u %s%s",
            (uint32_t)entity,
            isActive ? "[ACTIVE] " : "",
            isEditor ? "[EDITOR]" : "");

        // --- Actions ---
        if (!isActive) {
            if (ImGui::Button("Set Active")) {
                setActiveCamera(Entity{entity, this});
            }
        }
        else {
            ImGui::TextDisabled("Active Camera");
        }

        ImGui::SameLine();

        if (!isEditor) {
            if (ImGui::Button("Set Editor")) {
                m_EditorCamera = entity;
            }
        }
        else {
            ImGui::TextDisabled("Editor Camera");
        }

        // --- Transform ---
        ImGui::SliderFloat3("Position", &transform.position.x, -100.0f, 100.0f);

        // --- Rotation ---
        auto& ang = angles[entity];

        if (!initialized.contains(entity)) {
            glm::vec3 forward = transform.getForward();
            ang.x = glm::degrees(atan2(forward.z, forward.x));
            ang.y = glm::degrees(asin(glm::clamp(-forward.y, -1.0f, 1.0f)));
            initialized.insert(entity);
        }

        bool changed = false;
        changed |= ImGui::SliderFloat("Yaw", &ang.x, -180.0f, 180.0f);
        changed |= ImGui::SliderFloat("Pitch", &ang.y, -89.0f, 89.0f);

        if (changed) {
            glm::vec3 forward;
            forward.x = cos(glm::radians(ang.x)) * cos(glm::radians(ang.y));
            forward.y = sin(glm::radians(ang.y));
            forward.z = sin(glm::radians(ang.x)) * cos(glm::radians(ang.y));

            transform.lookAt(glm::normalize(forward));

            if (isActive) {
                m_PlayerController->syncFromCamera();
            }
        }

        ImGui::SliderFloat3("Scale", &transform.scale.x, 0.0f, 100.0f);

        ImGui::PopID();
    }
}

Entity World::getActiveCamera() const {
    return Entity(m_ActiveCamera, const_cast<World*>(this));
}

Entity World::getEditorCamera() const {
    return Entity(m_EditorCamera, const_cast<World*>(this));
}

void World::setActiveCamera(Entity newCamera) {
    m_ActiveCamera = newCamera.m_EntityHandle;

    m_PlayerController->setCamera(newCamera);
}

std::vector<Entity> World::getCameras() const {
    std::vector<Entity> cameras;
    auto view = m_Registry.view<Camera3D>();

    for (auto entity : view) {
        cameras.push_back(Entity(entity, const_cast<World*>(this)));
    }

    return cameras;
}

void World::swapCameras() {
    std::swap(m_ActiveCamera, m_EditorCamera);

    m_PlayerController->setCamera(getActiveCamera());
}

} // namespace zk
