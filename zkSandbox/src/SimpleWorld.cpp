#include "SimpleWorld.hpp"

#include "ZkEngine.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Renderer/Model.hpp"
#include "imgui.h"

void SimpleWorld::init() {
    auto cubeMap = zk::Asset::loadTextureCube({
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg",
    });

    // zk::Renderer::setSkybox(cubeMap);

    // auto shader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");
    // auto windowTexture =
    //     zk::Asset::loadTexture("textures/blending_transparent_window.png",
    //     zk::TextureFiltering::LINEAR);

    auto shader = zk::Asset::loadShader("shaders/Reflect.vert", "shaders/Reflect.frag");
    redMaterial = zk::Asset::loadMaterial({shader});
    std::shared_ptr<zk::Mesh> cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);
    // redMaterial->setColor({0.0f, 1.0f, 0.0f, 0.2f});
    redMaterial->setBlendMode(zk::BlendMode::TRANSPARENT);

    // // Player
    // m_Player = std::make_unique<Player>(*this);

    // Green cube
    auto greenShader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");
    auto greenMaterial = zk::Asset::loadMaterial({greenShader});
    greenMaterial->setColor({0.0f, 1.0f, 0.0f, 1.0f});

    auto greenCube = createEntity();
    greenCube.addRenderable(cubeMesh, greenMaterial);
    greenCube.setPosition({10.0f, 10.0f, 0.0f});
    greenCube.setScale({5.0f, 5.0f, 5.0f});
    // ----------

    auto mustangModel = zk::Asset::loadModel("models/wolf/wolf.gltf");
    // auto mustangModel = zk::Asset::loadModel("models/mustang.obj");
    // auto backpackModel = zk::Asset::loadModel("models/backpack/backpack.obj");

    auto explodeShader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");
    // auto explodeShader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag",
    // "shaders/Geometry.geom");

    backpack.addModel(mustangModel);
    backpack.setPosition({0.0f, 0.0f, -5.0f});
    backpack.setScale({5.0f, 5.0f, 5.0f});

    auto dirt2 = createEntity();
    dirt2.addRenderable(cubeMesh, redMaterial);
    dirt2.setPosition({10.0f, 0.0f, 0.0f});
    dirt2.setScale({5.0f, 5.0f, 5.0f});

    auto dirt3 = createEntity();
    dirt3.addRenderable(cubeMesh, redMaterial);
    dirt3.setPosition({10.0f, 0.0f, 1.0f});
    dirt3.setScale({5.0f, 5.0f, 5.0f});

    auto normalMaterial = zk::Asset::loadMaterial({shader});

    // Dirt
    auto dirtShader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");

    zk::TextureSpecs dirtSpecs;
    dirtSpecs.filter = zk::TextureFilter::Nearest;
    auto dirtTexture = zk::Asset::loadTexture("textures/default_dirt.png", dirtSpecs);
    // auto dirtTexture = zk::Asset::loadTexture("textures/UV_MAP1K.jpg");
    auto dirtMaterial =
        zk::Asset::loadMaterial(zk::MaterialSpecs{.shader = dirtShader, .diffuse = dirtTexture});

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                auto dirt = createEntity();
                dirt.addRenderable(cubeMesh, dirtMaterial);

                float xpos = (j + 1) * 10.0f;
                float ypos = (k + 1) * -10.0f;
                float zpos = (i + 1) * 10.0f;
                dirt.setPosition({xpos, ypos, zpos});
                dirt.setScale({5.0f, 5.0f, 5.0f});
                dirt.setRotation({0.0f, 20.0f * i, 0.0f});
            }
        }
    }

    rigidBody = createEntity();
    rigidBody.addRenderable(cubeMesh, greenMaterial);
    rigidBody.setPosition({0.0f, 20.0f, 0.0f});
    rigidBody.addRigidBody();
}

void SimpleWorld::update(float dt) {
    redMaterial->setColor({0.0f, 1.0f, 0.0f, alpha});

    rigidBody.getComponent<zk::RigidBody>().addForce({0.0f, -9.8f, 0.0f});

    // m_Player->onUpdate(dt);
    // auto cameraPos = getCamera()->getPosition();
    // m_Player->getEntity().setPosition(cameraPos);
    //
    // auto forward = getCamera()->getFront();
    // glm::mat4 look = glm::lookAt(glm::vec3(0.0f), forward, glm::vec3(0, 1, 0));
    // glm::quat rot = glm::quat_cast(glm::inverse(look));
    //
    // m_Player->getEntity().setRotation(rot);
    // m_Player->getEntity().setRotation({getCamera()->getPitch(), -getCamera()->getYaw(), 0.0f});
}

void SimpleWorld::debug() {
    ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f);
}
