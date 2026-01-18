#include "ZkEngine/Core/Application.hpp"

#include <print>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Input/Input.hpp"
#include "ZkEngine/Utils/Time.hpp"
#include "imgui.h"

Application::Application() {
    init();
}

Application::~Application() {
    shutdown();
}

void Application::init() {
    WindowSpecs specs;
    specs.title = "OpenGL Engine";
    specs.width = 1280;
    specs.height = 720;
    specs.vsync = false;

    m_Window = std::make_shared<Window>(specs);
    m_Window->create();

    m_Renderer.init();

    m_ImGuiLayer.init(m_Window->getWindow());

    Time::init();
}

void Application::run() {
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

void Application::update(float dt) {
    m_Renderer.update();

    // Init Scene
    if (Input::IsKeyPressed(Key::I) && !m_Scene) {
        m_Scene = std::make_unique<Scene>(false);
    }

    // Reset Scene
    if (Input::IsKeyPressed(Key::R) && m_Scene) {
        std::println("Resetting scene");
        CameraState cameraState = m_Scene->getCamera().getState();
        m_Scene.reset();
        m_Scene = std::make_unique<Scene>(false);

        m_Scene->getCamera().setState(cameraState);
    }

    updateScene(dt);
}

void Application::updateScene(float dt) {
    if (!m_Scene)
        return;

    // Update projection matrix (aspect ratio)
    glm::vec2 fbSize = m_Window->getSize();
    m_Scene->getCamera().onWindowResize(fbSize.x, fbSize.y);

    m_Scene->update(dt);
    if (m_Window->isCursorLocked()) {
        m_Scene->getCamera().onKeyPress(dt);
        m_Scene->getCamera().onMouseMove();
    }

    m_Scene->getCamera().updateView();

    // Toggle cursor mode
    if (Input::IsKeyPressed(Key::ESCAPE)) {
        // m_Window->setCursorLocked(!m_Window->isCursorLocked());
        int mode = m_Window->getCursorMode();

        if (mode == GLFW_CURSOR_DISABLED) {
            m_Window->setCursorLocked(false);
            m_Scene->getCamera().setFirstMouse(true);
        } else {
            m_Window->setCursorLocked(true);
        }
    }
}

void Application::render() {
    m_Renderer.beginFrame();

    if (m_Scene)
        m_Scene->submit(m_Renderer);

    m_Renderer.endFrame();

    m_ImGuiLayer.beginFrame();
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Debug");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        m_Renderer.debug();
        ImGui::SliderFloat("Simulation Speed", &m_SimulationSpeed, 0.1f, 10.0f);
        Time::setTimeScale(m_SimulationSpeed);

        if (m_Scene)
            m_Scene->debug();
        ImGui::End();

        drawCrosshair();
    }

    m_ImGuiLayer.endFrame();
}

void Application::shutdown() {
    m_ImGuiLayer.shutdown();
    m_Window.reset();
}

void Application::drawCrosshair() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);

    float size = 8.0f;
    ImU32 color = IM_COL32(255, 255, 255, 255);

    drawList->AddLine({center.x - size, center.y}, {center.x + size, center.y}, color, 1.5f);
    drawList->AddLine({center.x, center.y - size}, {center.x, center.y + size}, color, 1.5f);
}
