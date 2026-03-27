#include "Scene.hpp"

#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/ModelSystem.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace zk {

Scene::Scene() {}

Scene::~Scene() {}

void Scene::init() {}

// Manage all rigid bodies
void Scene::update(float dt) {
    for (auto& [entity, rigidBody] : m_Registry.getStorage<RigidBody>()) {
        if (!m_Registry.hasComponent<Transform3D>(entity)) {
            continue;
        }

        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        // rigidBody.update(dt);
        transform.position = rigidBody.position;
        transform.rotation = rigidBody.rotation;
    }

    // m_PhysicsSystem.update(m_Registry, *m_Camera, dt);
}

// Manage all renderable objects
void Scene::submit(RendererBackend& renderer) const {
    for (auto& [entity, renderObject] : m_Registry.getStorage<RenderObject>()) {
        if (!m_Registry.hasComponent<Transform3D>(entity)) {
            // ZK_CORE_WARN("No transform");
            continue;
        }

        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        RenderCommand cmd;
        cmd.mesh = renderObject.mesh.get();
        cmd.material = renderObject.material.get();
        cmd.modelMatrix = transform.getMatrix();
        cmd.distanceToCamera = glm::length2(m_Camera.getPosition() - transform.position);
        renderer.submit(cmd);
    }
}

void Scene::updateModelSystem(RendererBackend& renderer) {
    ModelSystem::update(*this, renderer);
}

LightData Scene::getLights() const {
    LightData lightData;

    // Point Lights
    for (auto& [entity, pointLight] : m_Registry.getStorage<PointLight>()) {
        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        lightData.pointLights.emplace_back(pointLight, transform.position);
    }

    // Spot Lights
    for (auto& [entity, spotLight] : m_Registry.getStorage<SpotLight>()) {
        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        lightData.spotLights.emplace_back(spotLight, transform.position);
    }

    // Directional Lights
    for (auto& [entity, dirLight] : m_Registry.getStorage<DirectionalLight>()) {
        lightData.dirLights.emplace_back(dirLight);
    }

    return lightData;
}

// void LightSystem::uploadTo(Shader& shader) {
//     shader.bind();
//
//     shader.setInt("u_PointLightCount", m_PointLights.size());
//
//     for (size_t i = 0; i < m_PointLights.size(); i++) {
//         const auto& pointLight = m_PointLights[i];
//
//         shader.setLight(
//             "u_PointLights[" + std::to_string(i) + "]", pointLight.light, pointLight.position);
//
//         // Bind shadow map
//         int unit = 4 + i;
//         glActiveTexture(GL_TEXTURE0 + unit);
//         glBindTexture(GL_TEXTURE_CUBE_MAP, pointLight.depthMap);
//
//         shader.setInt("u_ShadowCubeMaps[" + std::to_string(i) + "]", unit);
//         shader.setFloat("u_FarPlanes[" + std::to_string(i) + "]", pointLight.farPlane);
//     }
//
//     for (auto& spotLight : m_SpotLights) {
//         // NOTE: Remember this is passing as u_SpotLight and not u_SpotLight[i]
//         shader.setLight("u_SpotLight", spotLight.light, spotLight.position);
//     }
//
//     for (auto& dirLight : m_DirLights) {
//         shader.setLight("u_DirLight", dirLight);
//     }
// }

void Scene::debug() {}

// void LightSystem::debug() {
//     ImGui::Text("Light System");
//     if (ImGui::TreeNode("PointLights")) {
//         auto& pointLights = m_Registry.getStorage<PointLight>();
//
//         ImGui::Text("Point Lights: %zu", pointLights.size());
//         ImGui::PushID("PointLights");
//         for (auto& [entity, pointLight] : pointLights) {
//             ImGui::PushID(entity);
//             ImGui::Text("Point Light %u", entity);
//             auto& position = m_Registry.getComponent<Transform3D>(entity).position;
//             ImGui::SliderFloat3("Position", &position.x, -100.0f, 100.0f);
//             ImGui::SliderFloat3("Color", &pointLight.color.x, 0.0f, 1.0f);
//             ImGui::SliderFloat("Linear", &pointLight.linear, 0.0f, 1.0f);
//             ImGui::SliderFloat("Quadratic", &pointLight.quadratic, 0.0f, 0.5f);
//             ImGui::SliderFloat("Ambient", &pointLight.ambient, 0.0f, 1.0f);
//             ImGui::SliderFloat("Diffuse", &pointLight.diffuse, 0.0f, 1.0f);
//             ImGui::SliderFloat("Specular", &pointLight.specular, 0.0f, 1.0f);
//             ImGui::PopID();
//         }
//         ImGui::PopID();
//         ImGui::TreePop();
//     }
//
//     if (ImGui::TreeNode("DirLights")) {
//         auto& dirLights = m_Registry.getStorage<DirectionalLight>();
//
//         ImGui::Text("Directional Lights: %zu", dirLights.size());
//         ImGui::PushID("DirectionalLights");
//         for (auto& [entity, dirLight] : dirLights) {
//             ImGui::PushID(entity);
//             ImGui::Text("Directional Light %u", entity);
//             ImGui::SliderFloat3("Direction", &dirLight.direction.x, -1.0f, 1.0f);
//             ImGui::SliderFloat3("Color", &dirLight.color.x, 0.0f, 1.0f);
//             ImGui::PopID();
//         }
//         ImGui::PopID();
//         ImGui::TreePop();
//     }
//
//     if (ImGui::TreeNode("SpotLights")) {
//         auto& spotLights = m_Registry.getStorage<SpotLight>();
//
//         ImGui::Text("Spot Lights: %zu", spotLights.size());
//         ImGui::PushID("SpotLights");
//         for (auto& [entity, spotLight] : spotLights) {
//             ImGui::PushID(entity);
//             ImGui::Text("Spot Light %u", entity);
//             // ImGui::SliderFloat3("Position", &spotLight.position.x, -100.0f, 100.0f);
//             ImGui::SliderFloat3("Direction", &spotLight.direction.x, -1.0f, 1.0f);
//             ImGui::SliderFloat3("Color", &spotLight.color.x, 0.0f, 1.0f);
//             ImGui::SliderFloat("Inner", &spotLight.cutOff, 0.0f, 1.0f);
//             ImGui::SliderFloat("Outer", &spotLight.outerCutOff, 0.0f, 1.0f);
//             ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
//             ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 0.5f);
//             ImGui::PopID();
//         }
//         ImGui::PopID();
//         ImGui::TreePop();
//     }
// }


} // namespace zk
