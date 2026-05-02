#include "PhysicsWorld.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Input/Input.hpp"
#include "imgui.h"

// TODO:
// Get the shader from the AssetManager
// m_LightingShader = AssetManager::get<zk::Shader>("lighting");
// m_FloorTexture = AssetManager::get<zk::Texture>("floorTexture");
// Get a method like zk::CreateLight()

// void PhysicsWorld::init() {
//     loadCubes();
// }

void PhysicsWorld::init() {
    m_CubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);

    m_LightingShader = GetAssets().loadShader("shaders/lighting.vert", "shaders/multipleLightsShadow.frag");

    // m_LightingShader = Assets().loadShader("shaders/lighting.vert", "shaders/multipleLights.frag");
    // m_LightingShader = Assets().loadShader("shaders/light.vert", "shaders/light.frag");
    // m_LightingShader = Assets().loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    // m_FloorTexture = Assets().loadTexture("textures/rubber_tiles.png", zk::TextureFiltering::LINEAR);
    m_FloorTexture = GetAssets().loadTexture("textures/brickwall/brickwall.jpg");
    auto normalFloor = GetAssets().loadTexture("textures/brickwall/brickwall_normal.jpg");
    // m_FloorTexture = Assets().loadTexture("textures/wood.png");

    // auto brick = GetAssets().loadTexture("textures/wood.png");
    // auto brickNormal = GetAssets().loadTexture("textures/wood_normal.png");
    // auto brickDepth = GetAssets().loadTexture("textures/wood_disp.png");
    // m_FloorMaterial = GetAssets().loadMaterial(
    //     zk::MaterialSpecs{
    //         .shader = m_LightingShader,
    //         .diffuse = brick,
    //         .normal = brickNormal,
    //         .height = brickDepth,
    //     });

    m_FloorMaterial = GetAssets().loadMaterial(
        zk::MaterialSpecs{
            .shader = m_LightingShader,
            .diffuse = m_FloorTexture,
            .normal = normalFloor,
        });

    buildFloor();

    buildLights();

    auto grassTexture = GetAssets().loadTexture("textures/grass.png");
    auto grassMaterial = GetAssets().loadMaterial(
        zk::MaterialSpecs{
            .shader = m_LightingShader,
            .diffuse = grassTexture,
        });

    // auto canonModel = Assets().loadModel("models/trueno/scene.gltf");
    // zk::Entity camera = createEntity();
    // // camera.addRenderable(m_CubeMesh, grassMaterial);
    // camera.addModel(canonModel, m_LightingShader);
    // camera.setPosition({0.0f, 0.0f, -20.0f});
    // camera.setScale({1.0f, 1.0f, 1.0f});

    auto tree = createEntity();
    auto treeModel = GetAssets().loadModel("models/Tree/Tree.obj");

    tree.addModel(treeModel, m_LightingShader);
    tree.setPosition({0.0f, -2.0f, 15.0f});
    tree.setScale({6.0f, 6.0f, 6.0f});

    auto backpack = createEntity();
    auto backpackModel = GetAssets().loadModel("models/backpack/backpack.obj");
    backpack.addModel(backpackModel, m_LightingShader);
    backpack.setPosition({0.0f, 4.0f, -15.0f});
    // backpack.setScale({6.0f, 6.0f, 6.0f});

    // m_Player = std::make_unique<Player>(*this);

    // auto sponzaModel = Assets().loadModel("models/Sponza/Sponza.obj");
    // zk::Entity sponza = createEntity();
    // sponza.addModel(sponzaModel, m_LightingShader);

    // sponza.setScale({1.0f, 1.1f, 0.1f});
    // zk::Transform3D& tSponza = sponza.addComponent<zk::Transform3D>();
    // tSponza.scale = glm::vec3(0.1f);
    // tSponza.position = glm::vec3(200.0f, 20.0f, 0.0f);

    // tSponza.scale = glm::vec3(1.0f);
    // tSponza.position = glm::vec3(0.0f, 20.0f, 0.0f);
}

void PhysicsWorld::update(float dt) {
    // if (getCamera()->getPosition().y <= 1.7f) {
    //     getCamera()->setPosition({getCamera()->getPosition().x, 1.7f, getCamera()->getPosition().z});
    // }
    // m_Player->onUpdate(dt);

    distance += zk::Input::GetMouseScroll() * 5.0f;
    distance = glm::clamp(distance, 20.0f, 100.0f);

    // m_Camera->setPosition(player.getRenderObject().transform.position -
    //                       m_Camera->getFront() * distance);

    // Update Spotlight
    // m_SpotLight.setPosition(getActiveCamera().getPosition());
    // m_SpotLight.getSpotLight().direction = getActiveCamera().getComponent<zk::Transform3D>().getForward();
    // float flicker = 0.9f + sin(zk::Time::getTime() * 10.0f) * 0.05f;
    // m_SpotLight.getSpotLight().color = glm::vec3(1.0f) * flicker;

    posX += 0.6f * dt;
    // m_Entity.getComponent<zk::Transform3D>().position = {-posX, 0.0f, 0.0f};
    // m_LightSource.getComponent<zk::Transform3D>().position = {posX, 10.0f, 0.0f};
}

void PhysicsWorld::buildFloor() {
    auto wall = createEntity();
    wall.addRenderable(m_CubeMesh, m_FloorMaterial);
    wall.setPosition({0.0f, 10.0f, -20.0f});

    // Add Floor
    auto dirt = createEntity();
    dirt.addRenderable(m_CubeMesh, m_FloorMaterial);
    dirt.setPosition({0.0f, -2.0f, 0.0f});
    dirt.setScale({100.0f, 0.1f, 100.0f});

    // NOTE:
    // Could do a std::vector<zk::Entity> and iterate over them? or maybe do a debug function in the
    // engine

    m_BoxTexture = zk::Asset::loadTexture("textures/container2.png");              // nearest
    m_BoxSpecularMap = zk::Asset::loadTexture("textures/container2_specular.png"); // nearest

    m_BoxMaterial = zk::Asset::loadMaterial(
        zk::MaterialSpecs{.shader = m_LightingShader, .diffuse = m_BoxTexture, .specular = m_BoxSpecularMap});
    m_BoxMaterial->setShininess(64.0f);

    // Floating Boxes
    for (int i = 0; i < 3; i++) {
        auto box = createEntity();
        box.addRenderable(m_CubeMesh, m_BoxMaterial);
        box.setPosition({7.0f * i, 10.0f, 0.0f});
        box.setScale({5.0f, 5.0f, 5.0f});
    }

    // Boxes
    // for (int i = 0; i < 3; i++) {
    //     auto dirt = createEntity();
    //     dirt.addRenderable(m_CubeMesh, m_BoxMaterial);
    //     dirt.setPosition({7.0f * i, 1.0f, 0.0f});
    //     dirt.setScale({5.0f, 5.0f, 5.0f});
    //     dirt.setRotation({0.0f, 20.0f * i, 0.0f});
    // }
}

void PhysicsWorld::buildLights() {
    // TODO:
    // Make scene darker (quadratic falloff helps here)
    // Could do color and intensity and then:
    // vec3 diffuse  = light.color * light.intensity;
    // vec3 ambient  = diffuse * 0.2;
    // vec3 specular = diffuse;

    // Flashlight
    m_SpotLight = createEntity();
    m_SpotLight.setPosition(getActiveCamera().getPosition());
    m_SpotLight.addSpotLight({
        .direction = getActiveCamera().getComponent<zk::Transform3D>().getForward(),
        .cutOff = glm::cos(glm::radians(12.5f)),
        .outerCutOff = glm::cos(glm::radians(17.5f)),
        .ambient = 0.2f,
        .diffuse = 0.8f,
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = m_LightLinear,
        .quadratic = m_LightQuadratic,
    });

    m_DirLight = createEntity();
    m_DirLight.addDirectionalLight({
        .direction = m_LightDir,
    });

    m_LightSourceShader = zk::Asset::loadShader("shaders/lighting.vert", "shaders/lightSource.frag");
    m_LightSourceMaterial = zk::Asset::loadMaterial({m_LightSourceShader});

    // Point Light
    m_LightSource = createEntity();
    m_LightSource.addRenderable(m_CubeMesh, m_LightSourceMaterial);
    m_LightSource.addPointLight({
        .linear = m_LightLinear,
        .quadratic = m_LightQuadratic,
    });

    m_LightSource.setPosition(lightSourcePos);
    m_LightSource.setScale({0.8f, 0.8f, 0.8f});

    // m_Entity = zk::createEntity();
    // m_Entity.addRenderable(m_CubeMesh, m_LightSourceMaterial);
    m_Entity.setPosition(lightSourcePos);
    m_Entity.setScale({0.8f, 0.8f, 0.8f});
    // m_Entity.addPointLight({
    //     .color = glm::vec3(1.0f, 0.0f, 0.0f),
    //     .linear = m_LightLinear,
    //     .quadratic = m_LightQuadratic,
    // });
}

void PhysicsWorld::loadCubes() {
    auto cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Cube);
    auto dirtShader = GetAssets().loadShader("shaders/Basic.vert", "shaders/Basic.frag");

    zk::TextureSpecs dirtSpecs;
    dirtSpecs.filter = zk::TextureFilter::Nearest;
    auto dirtTexture = GetAssets().loadTexture("textures/default_dirt.png", dirtSpecs);
    // auto dirtTexture = Assets().loadTexture("textures/UV_MAP1K.jpg");
    auto dirtMaterial = GetAssets().loadMaterial(zk::MaterialSpecs{.shader = dirtShader, .diffuse = dirtTexture});

    for (int i = 0; i < m_CubeCount; i++) {
        for (int j = 0; j < m_CubeCount; j++) {
            for (int k = 0; k < m_CubeCount; k++) {
                auto dirt = createEntity();
                dirt.addRenderable(cubeMesh, dirtMaterial);

                float xpos = (j + 1);
                float ypos = (k + 1);
                float zpos = (i + 1);
                dirt.setPosition({xpos, ypos, zpos});
                dirt.setScale({1.0f, 1.0f, 1.0f});
                // dirt.setRotation({0.0f, 20.0f * i, 0.0f});
            }
        }
    }
}

void PhysicsWorld::debug() {
    if (ImGui::SliderInt("Cube Count", &m_CubeCount, 1, 100)) {
        clear();
        loadCubes();
    }
}
