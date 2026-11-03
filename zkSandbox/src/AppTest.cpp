#include "AppTest.hpp"

#include "SponzaWorld.hpp"
#include "ZkEngine.hpp"

#include "PhysicsWorld.hpp"
#include "ElectronWorld.hpp"
#include "BallWorld.hpp"
#include "SimpleWorld.hpp"
#include "MinecraftWorld.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "imgui.h"

AppTest::AppTest() {
    ZK_INFO("AppTest constructor");
}

void AppTest::onInit() {
    // zk::GetWorldManager().setWorld<ElectronWorld>();
    // zk::GetWorldManager().setWorld<BallWorld>();
    // getWorldManager().setWorld<MinecraftWorld>();

    // getWorldManager().setWorld<SimpleWorld>();
    getWorldManager().setWorld<PhysicsWorld>();
    // getWorldManager().setWorld<SponzaWorld>();
}

void AppTest::onUpdate(float dt) {
    zk::World& world = getWorldManager().getWorld();

    // if (zk::Input::IsKeyPressed(Key::F)) {
    //     getWorldManager().setWorld<MCWorld>();
    // }
    //
    // if (zk::Input::IsKeyPressed(Key::E)) {
    //     getWorldManager().setWorld<PhysicsWorld>();
    // }

    // Reload current world
    if (zk::Input::IsKeyPressed(Key::R)) {
        getWorldManager().reloadWorld();
    }

    if (zk::Input::IsKeyPressed(Key::P)) {
        world.swapCameras();
    }

    if (zk::Input::IsKeyPressed(Key::H)) {
        zk::ToggleDebugWindow();
    }

    // Toggle cursor mode
    if (zk::Input::IsKeyPressed(Key::ESCAPE)) {
        bool locked = zk::IsCursorLocked();

        zk::LockCursor(!locked);

        if (locked) {
            // world.getActiveCamera().getController().captureMouse(false);
            world.getPlayerController().captureMouse(false);
        }
        else {
            // world.getActiveCamera().getController().captureMouse(true);
            world.getPlayerController().captureMouse(true);
        }
    }
}

void AppTest::onRender(zk::RendererBackend& renderer) {
    // zk::Renderer::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    // getCurrentWorld().submit(renderer);
}

void AppTest::onImGuiRender() {
    ImGui::Begin("AppTest");

    // Select World
    if (ImGui::Combo(
            "World",
            &m_CurrentWorld,
            "PhysicsWorld\0ElectronWorld\0BallWorld\0SimpleWorld\0MinecraftWorld\0SponzaWorld\0\0")) {
        if (m_CurrentWorld == 1) {
            getWorldManager().setWorld<PhysicsWorld>();
        }
        else if (m_CurrentWorld == 2) {
            getWorldManager().setWorld<ElectronWorld>();
        }
        else if (m_CurrentWorld == 3) {
            getWorldManager().setWorld<BallWorld>();
        }
        else if (m_CurrentWorld == 4) {
            getWorldManager().setWorld<SimpleWorld>();
        }
        else if (m_CurrentWorld == 5) {
            getWorldManager().setWorld<MinecraftWorld>();
        }
        else if (m_CurrentWorld == 6) {
            getWorldManager().setWorld<SponzaWorld>();
        }
    }

    getWorldManager().getWorld().debug();

    ImGui::End();

    drawCrosshair();
}

void AppTest::drawCrosshair() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);

    float size = 2.0f;
    ImU32 color = IM_COL32(255, 255, 255, 255);

    // drawList->AddLine({ center.x - size, center.y }, { center.x + size, center.y }, color, 1.5f);
    // drawList->AddLine({ center.x, center.y - size }, { center.x, center.y + size }, color, 1.5f);
    drawList->AddCircleFilled(center, size, color);
}
