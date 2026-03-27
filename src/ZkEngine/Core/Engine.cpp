#include "ZkEngine/Core/Engine.hpp"

#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Input/Input.hpp"
#include "ZkEngine/Utils/Time.hpp"
#include "ZkEngine/Core/Base.hpp"
#include "imgui.h"
#include <GLFW/glfw3.h>

namespace zk {

Engine::Engine(const EngineSpecs& specs) {
    s_Instance = this;

    init(specs);
}

Engine::~Engine() {
    shutdown();
}

void Engine::init(const EngineSpecs& specs) {
    // TODO:
    // Manage this of the renderr inside the window like glfwFramebufferSizeCallback
    // and the glViewport stuff
    m_Window = std::make_unique<Window>(specs);
    m_Window->create();

    m_ImGuiLayer.init(m_Window->getWindow());

    m_Renderer = RendererBackend::create(RenderAPI::OpenGL);

    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path assetPath = exePath / "res";
    m_AssetManager = std::make_unique<AssetManager>(assetPath, &m_Renderer->getDevice());

    m_Renderer->init();

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

        if (m_CursorLocksEvent.size() > 0) {
            m_Window->setCursorLocked(m_CursorLocksEvent.back());
            m_CursorLocksEvent.pop_back();
        }
    }
}

void Engine::update(float dt) {
    m_Renderer->update();
    m_App->onUpdate(dt);
    m_SceneManager.getScene().getCameraController().update(dt);

    glm::vec2 fbSize = m_Window->getSize();
    // Only resize if dimensions actually changed
    if (fbSize.x != m_LastWidth || fbSize.y != m_LastHeight) {
        m_LastWidth = fbSize.x;
        m_LastHeight = fbSize.y;
        onWindowResize(fbSize.x, fbSize.y);
    }
}

void Engine::render() {
    Scene& scene = m_SceneManager.getScene();

    m_Renderer->renderScene(scene);

    if (m_HideDebugWindow)
        return;

    m_ImGuiLayer.beginFrame();
    {
        onImGuiRender();
        m_App->onImGuiRender();
    }
    m_ImGuiLayer.endFrame();
}

void Engine::onWindowResize(int width, int height) {
    // Update projection matrix (aspect ratio)
    glm::vec2 fbSize = m_Window->getSize();
    m_Renderer->resize(fbSize.x, fbSize.y);
    // m_Renderer->setViewport(0, 0, fbSize.x, fbSize.y);

    m_SceneManager.getScene().getCamera()->onWindowResize(fbSize.x, fbSize.y);
}

void Engine::shutdown() {
    m_ImGuiLayer.shutdown();

    m_Renderer.reset();

    m_AssetManager.reset();

    m_Window.reset();

    glfwTerminate();
}

void Engine::onImGuiRender() {
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
            if (ImGui::SliderFloat("Simulation Speed", &m_SimulationSpeed, 0.1f, 20.0f)) {
                Time::setTimeScale(m_SimulationSpeed);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Renderer")) {
            m_Renderer->debug();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Asset Manager")) {
            m_AssetManager->debug();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scene")) {
            m_SceneManager.debug();
            ImGui::EndTabItem();
        }

        // if (ImGui::BeginTabItem("App")) {
        //     m_App->onImGuiRender();
        //     ImGui::EndTabItem();
        // }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

} // namespace zk
