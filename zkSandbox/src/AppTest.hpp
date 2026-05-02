#pragma once

#include <ZkEngine/Core/Application.hpp>

class AppTest : public zk::Application {
  public:
    AppTest();

  protected:
    void onInit() override;
    void onUpdate(float dt) override;
    void onRender(zk::RendererBackend& renderer) override;
    void onImGuiRender() override;

  private:
    void drawCrosshair();

  private:
    int m_CurrentWorld = 0;
};
