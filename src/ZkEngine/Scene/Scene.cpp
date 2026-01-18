#include "Scene.hpp"

#include "ZkEngine/Input/Input.hpp"

#include <cmath>
#include <print>
#include <vector>
#include "ZkEngine/MeshFactory.hpp"
#include "ZkEngine/Model.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "ZkEngine/Renderer/Renderer.hpp"

#include "imgui.h"
#include "ZkEngine/Utils/Time.hpp"

Scene::Scene(bool floor) : m_PhysicsSystem(m_Camera) {
    if (floor)
        floorScene();
    else
        init();
}

Scene::~Scene() {}

void Scene::floorScene() {
    std::shared_ptr<Mesh> cubeMesh = MeshFactory::createCube();

    auto floorShader = std::make_shared<Shader>("../res/shaders/Basic.glsl");
    auto floorMaterial = std::make_shared<Material>(floorShader, nullptr);

    auto floor = std::make_unique<RenderObject>(cubeMesh, floorMaterial);

    floor->setColor({0.2f, 0.2f, 0.2f, 1.0f});
    floor->transform.position = glm::vec3(0);
    floor->transform.scale = glm::vec3(50.0f, 0.1f, 50.0f);
    addRenderObject(std::move(floor));
}

void Scene::init() {
    std::shared_ptr<Mesh> cubeMesh = MeshFactory::createCube();
    std::shared_ptr<Mesh> quadMesh = MeshFactory::createQuad();
    std::shared_ptr<Mesh> sphereMesh = MeshFactory::createSphere(2.5f);
    std::shared_ptr<Mesh> triangleMesh = MeshFactory::createTriangle();

    auto shader = std::make_shared<Shader>("../res/shaders/Basic.glsl");
    auto normalMaterial = std::make_shared<Material>(shader, nullptr);

    std::shared_ptr<Mesh> velocityMesh = MeshFactory::createLine();
    std::shared_ptr<Mesh> gravityMesh = MeshFactory::createLine();
    std::shared_ptr<Mesh> angularMesh = MeshFactory::createLine();

    // Dirt
    auto dirtShader = std::make_shared<Shader>("../res/shaders/vertex.glsl");
    auto dirtTexture = std::make_shared<Texture>("../res/textures/default_dirt.png");
    auto dirtMaterial = std::make_shared<Material>(dirtShader, dirtTexture);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                auto dirt = std::make_unique<RenderObject>(cubeMesh, dirtMaterial);
                float xpos = (j + 1) * 10.0f;
                float ypos = (k + 1) * -10.0f;
                float zpos = (i + 1) * 10.0f;
                dirt->transform.position = glm::vec3(xpos, ypos, zpos);
                dirt->setColor({0.8f, 0.3f, 0.8f, 1.0f});
                addRenderObject(std::move(dirt));
            }
        }
    }
    auto cube = std::make_unique<RenderObject>(cubeMesh, dirtMaterial);
    // cube->setColor({0.8f, 0.3f, 0.8f, 1.0f});
    cube->transform.position = glm::vec3(0.0f, 10.0f, -10.0f);

    toonShader = std::make_shared<Shader>("../res/shaders/Toon.glsl");
    auto toonMaterial = std::make_shared<Material>(toonShader, nullptr);

    auto toon = std::make_unique<RenderObject>(cubeMesh, normalMaterial);
    toon->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto cube2 = std::make_unique<RenderObject>(cubeMesh, toonMaterial);
    cube2->transform.position = glm::vec3(0.0f, 0.0f, -200.0f);
    cube2->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    cube2->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto cube3 = std::make_unique<RenderObject>(cubeMesh, toonMaterial);
    cube3->transform.position = glm::vec3(100.0f, 0.0f, -200.0f);
    cube3->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    cube3->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto quad = std::make_unique<RenderObject>(quadMesh, toonMaterial);
    quad->transform.position = glm::vec3(0.0f, 50.0f, -200.0f);
    quad->transform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    quad->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    auto triangle = std::make_unique<RenderObject>(triangleMesh, normalMaterial);
    triangle->setColor({0.8f, 0.3f, 0.8f, 1.0f});
    triangle->transform.position = glm::vec3(0.0f, 100.0f, 0.0f);
    triangle->transform.scale = glm::vec3(3.0f);

    auto sphere = std::make_unique<RenderObject>(sphereMesh, dirtMaterial);
    sphere->transform.position = glm::vec3(0.0f, 200.0f, 0.0f);
    sphere->setColor({0.8f, 0.3f, 0.8f, 1.0f});

    addRenderObject(std::move(cube));
    addRenderObject(std::move(cube2));
    addRenderObject(std::move(cube3));
    addRenderObject(std::move(quad));
    addRenderObject(std::move(toon));
    addRenderObject(std::move(triangle));
    addRenderObject(std::move(sphere));

    auto canelaModel = std::make_shared<Model>("../res/models/Canelita.obj");

    m_ModelObject = std::make_unique<ModelObject>(canelaModel, shader);

    sphereObject = std::make_unique<RenderObject>(sphereMesh, normalMaterial);
    negativeObject = std::make_unique<RenderObject>(sphereMesh, normalMaterial);
    m_PositiveParticle.object = sphereObject.get();
    m_PositiveParticle.charge = +1.0f;
    m_NegativeParticle.object = negativeObject.get();
    m_NegativeParticle.charge = -1.0f;
    m_PositiveParticle.object->setColor({1.0f, 0.0f, 0.0f, 1.0f});
    m_NegativeParticle.object->setColor({0.0f, 0.0f, 1.0f, 1.0f});

    m_PositiveParticle.velocity = glm::vec3(0.0, 0.0, -1.0);

    m_PositiveParticle.object->transform.position = glm::vec3(30.0, 50.0, 0.0);
    m_NegativeParticle.object->transform.position = glm::vec3(-30.0, 50.0, 0.0);
}

void Scene::update(float dt) {
    angle = fmod(angle, 360.0f);

    angle += 50.0f * dt;

    r = 0.5 + 0.5 * sin(Time::getTime() * 3.0);  // oscillates 0..1 over time

    toonShader->bind();

    toonShader->setUniform3fv("u_LightDir", glm::vec3(-1.0f, -1.0f, -1.0f));
    toonShader->setUniform3fv("u_LightColor", glm::vec3(1.0f));
    toonShader->setUniform3fv("u_ViewPos", m_Camera.getPosition());

    // m_VectorLine->transform.scale = glm::vec3(velocity.x, 0.0f, velocity.z);

    if (Input::IsKeyPressed(Key::P)) {
        std::println("Adding random object");
        // addRandomObject();
    }

    manageElectrons(dt);

    for (auto& obj : m_Objects) {
        if (m_Objects.back() == obj) {
            m_PhysicsSystem.update(*obj, dt);
            return;
        }

        obj->transform.rotation = glm::vec3(angle, 0.0f, angle);
        // obj->transform.rotation.y = angle;
    }
}

void Scene::addRenderObject(std::unique_ptr<RenderObject> renderObject) {
    m_Objects.push_back(std::move(renderObject));
}

void Scene::submit(Renderer& renderer) const {
    for (const auto& obj : m_Objects) {
        renderer.submit(*obj, m_Camera);
    }

    // m_ModelObject->draw(m_Camera.getViewProjection());

    // m_PhysicsSystem.drawDebugVectors();

    // m_PositiveParticle.object->draw(m_Camera.getViewProjection());
    // m_NegativeParticle.object->draw(m_Camera.getViewProjection());
}

void Scene::debug() {
    m_Camera.debug();
    m_PhysicsSystem.debug();

    for (size_t i = 0; i < m_Objects.size(); i++) {
        ImGui::PushID(i);

        ImGui::Text("Object %zu", i);
        ImGui::SliderFloat3("Translation", &m_Objects[i]->transform.position.x, -500, 500);
        ImGui::SliderFloat3("Scale", &m_Objects[i]->transform.scale.x, 0.1f, 20.0f);
        ImGui::SliderFloat3("Rotation", &m_Objects[i]->transform.rotation.x, 0.0f, 360.0f);

        ImGui::Separator();
        ImGui::PopID();
    }
}

void Scene::manageElectrons(float dt) {
    glm::vec3 direction = m_PositiveParticle.object->transform.position -
                          m_NegativeParticle.object->transform.position;
    float r = glm::length(direction);
    if (r < 1e-6f)
        return;

    glm::vec3 directionVec = glm::normalize(direction);
    // float forceMagnitude = K * (Q1 * Q2) / (r * r);
    float forceMagnitude = K * (m_PositiveParticle.charge * m_NegativeParticle.charge) / (r * r);

    // float forceMagnitude =
    //     COULOMB_CONSTANT * (std::abs(ELECTRON_CHARGE) * std::abs(ELECTRON_CHARGE)) / (r * r);
    glm::vec3 forceVector = directionVec * forceMagnitude;

    m_PositiveParticle.velocity += forceVector / MASS * dt;
    m_NegativeParticle.velocity -= forceVector / MASS * dt;

    m_PositiveParticle.object->transform.position += m_PositiveParticle.velocity * dt;
    m_NegativeParticle.object->transform.position += m_NegativeParticle.velocity * dt;

    std::println("Force: {}, Distance: {}", forceVector.x, r);
}
