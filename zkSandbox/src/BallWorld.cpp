#include "BallWorld.hpp"

#include "ZkEngine/World/Entity.hpp"
#include "ZkEngine/Utils/MeshFactory.hpp"
#include "ZkEngine/Utils/Time.hpp"
#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Input/Input.hpp"
#include "imgui.h"

void BallWorld::init() {
    std::shared_ptr<zk::Mesh> cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);
    std::shared_ptr<zk::Mesh> quadMesh = GetAssets().getMesh(zk::PrimitiveType::Quad);
    std::shared_ptr<zk::Mesh> sphereMesh = GetAssets().getMesh(zk::PrimitiveType::Sphere);
    std::shared_ptr<zk::Mesh> triangleMesh = GetAssets().getMesh(zk::PrimitiveType::Triangle);

    auto shader = zk::Asset::loadShader("shaders/Basic.vert", "shaders/Basic.frag");
    auto normalMaterial = zk::Asset::loadMaterial({shader});

    std::shared_ptr<zk::Mesh> velocityMesh = GetAssets().getMesh(zk::PrimitiveType::Line);
    std::shared_ptr<zk::Mesh> gravityMesh = GetAssets().getMesh(zk::PrimitiveType::Line);
    std::shared_ptr<zk::Mesh> angularMesh = GetAssets().getMesh(zk::PrimitiveType::Line);

    // Dirt
    auto dirtShader = zk::Asset::loadShader("shaders/vertex.vert", "shaders/vertex.frag");
    auto dirtTexture = zk::Asset::loadTexture("textures/default_dirt.png");
    auto dirtMaterial = zk::Asset::loadMaterial({dirtShader, dirtTexture});

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
                // dirt->setColor({0.8f, 0.3f, 0.8f, 1.0f});
            }
        }
    }

    auto cube = createEntity();
    cube.addRenderable(cubeMesh, normalMaterial);
    cube.setPosition({0.0f, 10.0f, -10.0f});
    // cube->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto toonShader = zk::Asset::loadShader("shaders/Toon.vert", "shaders/Toon.frag");
    auto toonMaterial = zk::Asset::loadMaterial({toonShader});

    auto toon = createEntity();
    toon.addRenderable(cubeMesh, toonMaterial);
    // toon->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto cube2 = std::make_unique<zk::RenderObject>(cubeMesh, toonMaterial);
    // cube2->transform.position = glm::vec3(0.0f, 0.0f, -200.0f);
    // cube2->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    // cube2->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto cube3 = std::make_unique<zk::RenderObject>(cubeMesh, toonMaterial);
    // cube3->transform.position = glm::vec3(100.0f, 0.0f, -200.0f);
    // cube3->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    // cube3->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto quad = std::make_unique<zk::RenderObject>(quadMesh, toonMaterial);
    // quad->transform.position = glm::vec3(0.0f, 50.0f, -200.0f);
    // quad->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    // quad->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto triangle = std::make_unique<zk::RenderObject>(triangleMesh, normalMaterial);
    // triangle->setColor({0.8f, 0.3f, 0.8f, 1.0f});
    // triangle->transform.position = glm::vec3(0.0f, 100.0f, 0.0f);
    // triangle->transform.scale = glm::vec3(3.0f);

    auto sphere = std::make_unique<zk::RenderObject>(sphereMesh, dirtMaterial);
    // sphere->transform.position = glm::vec3(0.0f, 200.0f, 0.0f);
    // sphere->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    // auto canelaModel = std::make_shared<zk::Model>(AssetManager::asset("models/Canelita.obj"));

    // m_ModelObject = std::make_unique<zk::ModelObject>(canelaModel, shader);
}

void BallWorld::update(float dt) {
    angle = fmod(angle, 360.0f);

    angle += 50.0f * dt;

    r = 0.5 + 0.5 * sin(Time::getTime() * 3.0); // oscillates 0..1 over time

    // m_VectorLine->transform.scale = glm::vec3(velocity.x, 0.0f, velocity.z);

    // if (zk::Input::IsKeyPressed(Key::P)) {
    //     std::println("Adding random object");
    //     // addRandomObject();
    // }
}

void BallWorld::debug() {
    // m_PhysicsSystem.debug();
}
