#pragma once

#include "ZkEngine/Core/Application.hpp"
#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/ImGui/ImGuiLayer.hpp"
#include "ZkEngine/Scene/SceneManager.hpp"

namespace zk {

class Engine {
  public:
    Engine(const EngineSpecs& specs);
    ~Engine();

    void run(Application& app);
    static Engine* Get() { return s_Instance; }

    Core::RendererBackend& getRenderer() { return *m_Renderer; }
    SceneManager& getSceneManager() { return m_SceneManager; }
    Window& getWindow() { return *m_Window; }

    void toggleDebugWindow() { m_HideDebugWindow = !m_HideDebugWindow; }

  private:
    void init(const EngineSpecs& specs);
    void update(float dt);
    void render();

    void onWindowResize(int width, int height);

    void shutdown();

    void onImGuiRender();

  private:
    inline static Engine* s_Instance = nullptr;

    Application* m_App = nullptr;

    std::unique_ptr<Core::RendererBackend> m_Renderer;
    std::unique_ptr<Window> m_Window;
    SceneManager m_SceneManager;

    ImGuiLayer m_ImGuiLayer;

    bool m_Running = true;

    float m_SimulationSpeed = 1.0f;

    bool m_HideDebugWindow = false;
};

} // namespace zk
