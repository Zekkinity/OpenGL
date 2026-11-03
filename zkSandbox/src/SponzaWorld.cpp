#include "SponzaWorld.hpp"

#include "ZkEngine/World/Entity.hpp"
#include <random>

static glm::vec3 getRandomPosition(const glm::vec3& min, const glm::vec3& max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> disX(min.x, max.x);
    std::uniform_real_distribution<float> disY(min.y, max.y);
    std::uniform_real_distribution<float> disZ(min.z, max.z);

    return {disX(gen), disY(gen), disZ(gen)};
}

void SponzaWorld::init() {
    auto sponzaModel = GetAssets().loadModel("models/sponza/Sponza.gltf");
    zk::Entity sponza = createEntity();
    sponza.addModel(sponzaModel);
    sponza.setScale(0.1f);

    // auto samuraiModel = GetAssets().loadModel("models/samuraiHelmet/scene.gltf");
    //
    // // Helmets
    // for (int i = 0; i < 10; i++) {
    //     const float radius = 20.0f;
    //     glm::vec3 position = getRandomPosition({-radius, 0.0f, -radius}, {radius, 10.0f, radius});
    //
    //     zk::Entity samurai = createEntity();
    //     samurai.addModel(samuraiModel, nullptr);
    //     samurai.setPosition(position);
    //     samurai.setRotation({-30.0f, 0.0f, 0.0f});
    // }
    //
    // // Lights
    // for (int i = 0; i < 16; i++) {
    //     const float radius = 20.0f;
    //     glm::vec3 position = getRandomPosition({-radius, 0.0f, -radius}, {radius, 10.0f, radius});
    //
    //     zk::Entity light = SpawnPointLight();
    //     light.setPosition(position);
    // }

    // std::string path = "textures/rustedIron/rustediron2_";
    // auto material = GetAssets().loadMaterial(
    //     zk::MaterialSpecs{
    //         .diffuse = GetAssets().loadTexture(path + "basecolor.png"),
    //         .normal = GetAssets().loadTexture(path + "normal.png"),
    //         .metallic = GetAssets().loadTexture(path + "metallic.png"),
    //         .roughness = GetAssets().loadTexture(path + "roughness.png"),
    //     });

    std::string path = "textures/Brick1/Bricks075A_1K-PNG_";
    auto material = GetAssets().loadMaterial(
        zk::MaterialSpecs{
            .diffuse = GetAssets().loadTexture(path + "Color.png"),
            .normal = GetAssets().loadTexture(path + "NormalGL.png"),
            .roughness = GetAssets().loadTexture(path + "Roughness.png"),
            .ao = GetAssets().loadTexture(path + "AmbientOcclusion.png"),
        });

    auto cubeMesh = GetAssets().getMesh(zk::PrimitiveType::Quad);
    auto dirt = createEntity();
    dirt.addRenderable(cubeMesh, material);
    dirt.setPosition({-10.0f, 0.0f, 0.0f});
    dirt.setScale(3.0f);

    
    SpawnDirLight();
}

void SponzaWorld::update(float dt) {}

void SponzaWorld::debug() {}
