#include "SponzaWorld.hpp"

#include "ZkEngine/World/Entity.hpp"

void SponzaWorld::init() {
    auto m_LightingShader = GetAssets().loadShader("shaders/lighting.vert", "shaders/multipleLightsShadow.frag");

    auto sponzaModel = GetAssets().loadModel("models/Sponza/Sponza.obj");
    zk::Entity sponza = createEntity();
    sponza.addModel(sponzaModel, m_LightingShader);
    sponza.setScale(0.1f);
}

void SponzaWorld::update(float dt) {}

void SponzaWorld::debug() {}
