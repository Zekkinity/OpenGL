#pragma once

#include "ZkEngine/Core/Application.hpp"
#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/ImGui/ImGuiLayer.hpp"
#include "ZkEngine/Asset/AssetManager.hpp"

namespace zk {

class Engine {
  public:
    Engine(const EngineSpecs& specs);
    ~Engine();

    void run(Application& app);
    static Engine* Get() { return s_Instance; }

    RendererBackend& getRenderer() {
        assert(m_Renderer && "Renderer not ready yet!");
        return *m_Renderer;
    }
    Window& getWindow() { return *m_Window; }
    AssetManager& getAssetManager() { return *m_AssetManager; }

    void toggleDebugWindow() { m_HideDebugWindow = !m_HideDebugWindow; }

    void lockCursor(bool locked) { m_CursorLocksEvent.push_back(locked); }
    bool isCursorLocked() { return m_Window->isCursorLocked(); }

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

    Scope<RendererBackend> m_Renderer;
    Scope<Window> m_Window;

    Scope<AssetManager> m_AssetManager;

    Scope<WorldManager> m_WorldManager;

    ImGuiLayer m_ImGuiLayer;

    bool m_Running = true;

    float m_SimulationSpeed = 1.0f;

    bool m_HideDebugWindow = false;

    float m_LastWidth = 0.0f;
    float m_LastHeight = 0.0f;

    std::vector<bool> m_CursorLocksEvent;
};

} // namespace zk
