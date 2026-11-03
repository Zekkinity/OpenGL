#include "MinecraftWorld.hpp"

#include "ZkEngine.hpp"

#include "imgui.h"

void MinecraftWorld::init() {
    m_LightShader = GetAssets().loadShader("shaders/lighting.vert", "shaders/multipleLightsShadow.frag");

    m_CubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);

    // buildDirt();

    buildFloor();
}

void MinecraftWorld::update(float dt) {}

void MinecraftWorld::buildFloor() {
    auto texture = GetAssets().loadTexture("textures/rubber_tiles.png");
    // auto texture = GetAssets().loadTexture("textures/UV_MAP1K.jpg");
    auto material = GetAssets().loadMaterial(
        zk::MaterialSpecs{
            .shader = m_LightShader,
            .diffuse = texture,
        });

    auto cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);

    auto floor = createEntity();
    floor.addRenderable(cubeMesh, material);
    floor.setScale({100.0f, 10.0f, 100.0f});

    for (int i = 0; i < 3; i++) {
        auto mat = GetAssets().loadMaterial(
            zk::MaterialSpecs{
                .shader = m_LightShader,
            }
        );
        auto box = createEntity();
        box.addRenderable(cubeMesh, mat);
        box.setPosition({7.0f * i, 10.0f, 0.0f});

        box.setColor({1.0f, 0.0f, 0.0f, 1.0f});
        box.setScale({1.0f, 1.0f, 1.0f});
    }

    auto grass = createEntity();
    auto model = GetAssets().loadModel("models/grass/grass.fbx");
    grass.addModel(model);
    grass.setScale({10.0f, 10.0f, 10.0f});
    grass.setPosition({0.0f, 30.0f, 0.0f});
}

void MinecraftWorld::buildDirt() {
    auto dirtShader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");
    auto dirtTexture = zk::Asset::loadTexture("textures/default_dirt.png", {.filter = zk::TextureFilter::Nearest});
    // auto dirtTexture = zk::Asset::loadTexture("textures/UV_MAP1K.jpg");

    auto material = zk::Asset::loadMaterial(
        zk::MaterialSpecs{
            .shader = dirtShader,
            .diffuse = dirtTexture,
        });

    auto cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                auto dirt = createEntity();
                dirt.addRenderable(cubeMesh, material);
                float xpos = (j + 1);
                float ypos = (k + 1);
                float zpos = (i + 1);
                dirt.setPosition({xpos, ypos, zpos});
            }
        }
    }
}

void MinecraftWorld::debug() {
    if (ImGui::SliderInt("Cube Count", &n, 1, 100)) {
        clear();
        buildDirt();
    }
}
