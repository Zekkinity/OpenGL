#include "ZkEngine/Scene/LightSystem.hpp"

#include "ZkEngine/Scene/Scene.hpp"
#include "ZkEngine/Shader.hpp"
#include "imgui.h"
#include <iostream>

namespace zk {

LightSystem::LightSystem(Scene& scene) : m_Scene(scene) {}

LightSystem::~LightSystem() {}

void LightSystem::upload(Shader& shader) {
    shader.bind();

    auto& pointLights = m_Scene.getStorage<PointLight>();
    shader.setUniform1i("u_PointLightCount", pointLights.size());

    size_t i = 0;
    for (auto& [entity, pointLight] : pointLights) {
        auto& transform = m_Scene.getComponent<Transform3D>(entity);

        shader.setLightUniform(
            "u_PointLights[" + std::to_string(i) + "]", pointLight, transform.position);

        i++;
    }

    auto& spotLights = m_Scene.getStorage<SpotLight>();
    for (auto& [entity, spotLight] : spotLights) {
        auto& transform = m_Scene.getComponent<Transform3D>(entity);

        // NOTE: Remember this is passing as u_SpotLight and not u_SpotLight[i]
        shader.setLightUniform("u_SpotLight", spotLight, transform.position);
    }

    auto& dirLights = m_Scene.getStorage<DirectionalLight>();
    for (auto& [entity, dirLight] : dirLights) {
        shader.setLightUniform("u_DirLight", dirLight);
    }
}

void LightSystem::debug() {
    ImGui::Text("Light System");
    if (ImGui::TreeNode("PointLights")) {
        auto& pointLights = m_Scene.getStorage<PointLight>();

        ImGui::Text("Point Lights: %zu", pointLights.size());
        ImGui::PushID("PointLights");
        for (auto& [entity, pointLight] : pointLights) {
            ImGui::PushID(entity);
            ImGui::Text("Point Light %u", entity);
            auto& position = m_Scene.getComponent<Transform3D>(entity).position;
            ImGui::SliderFloat3("Position", &position.x, -100.0f, 100.0f);
            // ImGui::SliderFloat3("Color", &m_PointLights[i].color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &pointLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &pointLight.quadratic, 0.0f, 2.0f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("DirLights")) {
        auto& dirLights = m_Scene.getStorage<DirectionalLight>();

        ImGui::Text("Directional Lights: %zu", dirLights.size());
        ImGui::PushID("DirectionalLights");
        for (auto& [entity, dirLight] : dirLights) {
            ImGui::PushID(entity);
            ImGui::Text("Directional Light %u", entity);
            ImGui::SliderFloat3("Direction", &dirLight.direction.x, -1.0f, 1.0f);
            // ImGui::SliderFloat3("Color", &m_DirectionalLights[i].color.x, 0.0f, 1.0f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SpotLights")) {
        auto& spotLights = m_Scene.getStorage<SpotLight>();

        ImGui::Text("Spot Lights: %zu", spotLights.size());
        ImGui::PushID("SpotLights");
        for (auto& [entity, spotLight] : spotLights) {
            ImGui::PushID(entity);
            ImGui::Text("Spot Light %u", entity);
            // ImGui::SliderFloat3("Position", &spotLight.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("Direction", &spotLight.direction.x, -1.0f, 1.0f);
            // ImGui::SliderFloat3("Color", &spotLight.color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Inner", &spotLight.cutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Outer", &spotLight.outerCutOff, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &spotLight.linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &spotLight.quadratic, 0.0f, 2.0f);
            ImGui::PopID();
        }
        ImGui::PopID();
        ImGui::TreePop();
    }
}

} // namespace zk
