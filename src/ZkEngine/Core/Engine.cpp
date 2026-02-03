#include "ZkEngine/Core/Engine.hpp"

#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Input/Input.hpp"
#include "ZkEngine/Utils/AssetManager.hpp"
#include "ZkEngine/Utils/Time.hpp"
#include "imgui.h"

namespace zk {

Engine::Engine(const EngineSpecs& specs) {
    s_Instance = this;

    init(specs);
}

Engine::~Engine() {
    shutdown();
}

void Engine::init(const EngineSpecs& specs) {
    m_Window = std::make_unique<Window>(specs);

    m_Window->create();

    m_Renderer = Core::RendererBackend::create(Core::RendererAPI::OpenGL);
    m_Renderer->init();

    m_ImGuiLayer.init(m_Window->getWindow());

    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path assetPath = exePath / "res";
    AssetManager::setRootPath(assetPath);

    Time::init();
}

void Engine::run(Application& app) {
    m_App = &app;
    m_App->onInit();

    while (m_Running) {
        Time::update();
        Input::Update();

        glfwPollEvents();

        if (m_Window->shouldClose()) {
            m_Running = false;
            break;
        }

        float dt = Time::getDeltaTime();
        update(dt);
        render();

        m_Window->update();
    }
}

void Engine::update(float dt) {
    m_Renderer->update();

    m_App->onUpdate(dt);
    onWindowResize(1920, 1080);
}

void Engine::render() {
    m_Renderer->beginFrame();

    m_App->onRender(*m_Renderer);

    m_Renderer->endFrame();

    if (m_HideDebugWindow)
        return;

    m_ImGuiLayer.beginFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::Begin("ZkEngine");

        ImGui::Text(
            "Engine average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Time")) {
                ImGui::Text("Time: %.2f", Time::getTime());
                ImGui::Text("Simulated time: %.2f", Time::getSimulatedTime());
                // if (ImGui::DragFloat("Simulation Speed", &m_SimulationSpeed, 0.02f, 0.1f, 20.0f)) {
                if (ImGui::SliderFloat("Simulation Speed", &m_SimulationSpeed, 0.1f, 20.0f)) {
                    Time::setTimeScale(m_SimulationSpeed);
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Renderer")) {
                m_Renderer->debug();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Scene")) {
                if (ImGui::Button("Reload Scene")) {
                    m_SceneManager.reloadScene();
                }

                static bool keepCameraState = m_SceneManager.getKeepCameraState();
                if (ImGui::Checkbox("Keep Camera State", &keepCameraState)) {
                    m_SceneManager.keepCameraState(keepCameraState);
                }
                ImGui::EndTabItem();
            }

            // if (ImGui::BeginTabItem("App")) {
            //     m_App->onImGuiRender();
            //     ImGui::EndTabItem();
            // }

            ImGui::EndTabBar();
        }

        m_App->onImGuiRender();

        ImGui::End();
    }

    m_ImGuiLayer.endFrame();
}

void Engine::onWindowResize(int width, int height) {
    // Update projection matrix (aspect ratio)
    glm::vec2 fbSize = m_Window->getSize();
    glViewport(0, 0, fbSize.x, fbSize.y);
    m_SceneManager.getScene().getCamera()->onWindowResize(fbSize.x, fbSize.y);
    m_SceneManager.getScene().getCamera()->updateView();
}

void Engine::shutdown() {
    m_ImGuiLayer.shutdown();
    m_Window.reset();
}

} // namespace zk
