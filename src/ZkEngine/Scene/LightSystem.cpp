#include "ZkEngine/Scene/LightSystem.hpp"

#include "ZkEngine/ECS/Registry.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "imgui.h"
#include <iostream>

namespace zk {

LightSystem::LightSystem(Registry& registry) : m_Registry(registry) {}

LightSystem::~LightSystem() {}

void LightSystem::upload(Shader& shader) {
    shader.bind();

    auto& pointLights = m_Registry.getStorage<PointLight>();
    shader.setUniform1i("u_PointLightCount", pointLights.size());

    size_t i = 0;
    for (auto& [entity, pointLight] : pointLights) {
        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        shader.setLightUniform(
            "u_PointLights[" + std::to_string(i) + "]", pointLight, transform.position);

        i++;
    }

    auto& spotLights = m_Registry.getStorage<SpotLight>();
    for (auto& [entity, spotLight] : spotLights) {
        auto& transform = m_Registry.getComponent<Transform3D>(entity);

        // NOTE: Remember this is passing as u_SpotLight and not u_SpotLight[i]
        shader.setLightUniform("u_SpotLight", spotLight, transform.position);
    }

    auto& dirLights = m_Registry.getStorage<DirectionalLight>();
    for (auto& [entity, dirLight] : dirLights) {
        shader.setLightUniform("u_DirLight", dirLight);
    }
}

void LightSystem::debug() {
    ImGui::Text("Light System");
    if (ImGui::TreeNode("PointLights")) {
        auto& pointLights = m_Registry.getStorage<PointLight>();

        ImGui::Text("Point Lights: %zu", pointLights.size());
        ImGui::PushID("PointLights");
        for (auto& [entity, pointLight] : pointLights) {
            ImGui::PushID(entity);
            ImGui::Text("Point Light %u", entity);
            auto& position = m_Registry.getComponent<Transform3D>(entity).position;
            ImGui::SliderFloat3("Position", &position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("Color", &pointLight.color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &pointLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &pointLight.quadratic, 0.0f, 0.5f);
            ImGui::SliderFloat("Ambient", &pointLight.ambient, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse", &pointLight.diffuse, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &pointLight.specular, 0.0f, 1.0f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("DirLights")) {
        auto& dirLights = m_Registry.getStorage<DirectionalLight>();

        ImGui::Text("Directional Lights: %zu", dirLights.size());
        ImGui::PushID("DirectionalLights");
        for (auto& [entity, dirLight] : dirLights) {
            ImGui::PushID(entity);
            ImGui::Text("Directional Light %u", entity);
            ImGui::SliderFloat3("Direction", &dirLight.direction.x, -1.0f, 1.0f);
            ImGui::SliderFloat3("Color", &dirLight.color.x, 0.0f, 1.0f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SpotLights")) {
        auto& spotLights = m_Registry.getStorage<SpotLight>();

        ImGui::Text("Spot Lights: %zu", spotLights.size());
        ImGui::PushID("SpotLights");
        for (auto& [entity, spotLight] : spotLights) {
            ImGui::PushID(entity);
            ImGui::Text("Spot Light %u", entity);
            // ImGui::SliderFloat3("Position", &spotLight.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("Direction", &spotLight.direction.x, -1.0f, 1.0f);
            ImGui::SliderFloat3("Color", &spotLight.color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Inner", &spotLight.cutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Outer", &spotLight.outerCutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 0.5f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }
}

} // namespace zk
