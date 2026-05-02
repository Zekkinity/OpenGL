#include "zkpch.hpp"

#include "ZkEngine/Core/Engine.hpp"

#include "ZkEngine/Utils/Profiler.hpp"

#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Input/Input.hpp"
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
    // TODO:
    // Manage this of the renderr inside the window like glfwFramebufferSizeCallback
    // and the glViewport stuff
    m_Window = createScope<Window>(specs);
    m_Window->create();
    m_Window->setVsync(false);

    m_ImGuiLayer.init(m_Window->getWindow());

    m_Renderer = RendererBackend::create(RenderAPI::OpenGL);

    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path assetPath = exePath / "res";
    m_AssetManager = createScope<AssetManager>(m_Renderer->getDevice(), assetPath);

    m_WorldManager = createScope<WorldManager>(m_Renderer->getDevice(), *m_AssetManager);

    m_Renderer->init();

    Time::init();
}

void Engine::run(Application& app) {
    m_App = &app;
    m_App->init(*m_WorldManager);

    while (m_Running) {
        ZK_PROFILE_SCOPE("Frame");

        Time::update();
        Input::Update();

        glfwPollEvents();

        if (m_Window->shouldClose()) {
            m_Running = false;
            break;
        }

        float dt = Time::getDeltaTime();
        {
            ZK_PROFILE_SCOPE("Update");
            update(dt);
        }

        {
            ZK_PROFILE_SCOPE("Render");
            render();
        }

        {
            ZK_PROFILE_SCOPE("Window Update");
            m_Window->update();
        }

        if (m_CursorLocksEvent.size() > 0) {
            m_Window->setCursorLocked(m_CursorLocksEvent.back());
            m_CursorLocksEvent.pop_back();
        }

        Profiler::endFrame();
    }
}

void Engine::update(float dt) {
    m_WorldManager->update(dt);
    m_App->onUpdate(dt);

    // m_Renderer->update();

    glm::vec2 fbSize = m_Window->getSize();
    // Only resize if dimensions actually changed
    if (fbSize.x != m_LastWidth || fbSize.y != m_LastHeight) {
        m_LastWidth = fbSize.x;
        m_LastHeight = fbSize.y;
        onWindowResize(fbSize.x, fbSize.y);
    }
}

void Engine::render() {
    World& world = m_App->getWorld();

    m_Renderer->beginFrame(world);
    {
        m_Renderer->render(world);
    }
    m_Renderer->endFrame();

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
    m_WorldManager->onWindowResize(width, height);

    // Update FBOs
    m_Renderer->resize(width, height);
}

void Engine::shutdown() {
    m_ImGuiLayer.shutdown();

    m_Renderer.reset();

    m_App = nullptr;
    m_AssetManager.reset();

    m_Window.reset();

    glfwTerminate();
}

void Engine::onImGuiRender() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::Begin("ZkEngine");

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("Time")) {
            ImGui::Text("Time: %.2f", Time::getTime());
            ImGui::Text("Simulated time: %.2f", Time::getSimulatedTime());
            if (ImGui::SliderFloat("Simulation Speed", &m_SimulationSpeed, 0.1f, 20.0f)) {
                Time::setTimeScale(m_SimulationSpeed);
            }

            static bool vsync = m_Window->isVsync();
            if (ImGui::Checkbox("Vsync", &vsync)) {
                m_Window->setVsync(vsync);
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

        if (ImGui::BeginTabItem("World")) {
            m_App->getWorldManager().debug();
            ImGui::EndTabItem();
        }

        // if (ImGui::BeginTabItem("App")) {
        //     m_App->onImGuiRender();
        //     ImGui::EndTabItem();
        // }

        ImGui::EndTabBar();
    }

    ImGui::End();

    Profiler::showStats();
}

} // namespace zk
