#pragma once

#include "ZkEngine/Core/Window.hpp"
#include "ZkEngine/Renderer/Renderer.hpp"

#include "ZkEngine/ImGui/ImGuiLayer.hpp"
#include "ZkEngine/Scene/Scene.hpp"

class Application {
   public:
    Application();
    ~Application();

    void run();

   private:
    void init();

    void update(float dt);
    void updateScene(float dt);

    void render();

    void shutdown();

    void drawCrosshair();

   private:
    std::shared_ptr<Window> m_Window;

    Renderer m_Renderer;
    std::unique_ptr<Scene> m_Scene;

    ImGuiLayer m_ImGuiLayer;

    bool m_Running = true;

    float m_SimulationSpeed = 1.0f;
};
