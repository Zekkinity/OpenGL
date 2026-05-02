#include "Player.hpp"

#include "ZkEngine/Input/Input.hpp"

Player::Player(zk::World& world) : m_PlayerEntity(world.createEntity()) {
    // auto cubeMesh = GetAss
    auto shader = zk::Asset::loadShader("shaders/lighting.vert", "shaders/lightSource.frag");

    auto material = zk::Asset::loadMaterial({shader});

    // m_PlayerEntity.addRenderable(cubeMesh, material);

    m_PlayerEntity.setPosition({20, 10, 20});
    m_PlayerEntity.setScale({0.5, 0.5, 0.5});
}

void Player::onUpdate(float dt) {
    if (zk::Input::IsKeyDown(Key::L)) {
        zk::Vec3 position = m_PlayerEntity.getPosition();
        position += zk::Vec3(1.0f, 0.0f, 0.0f) * m_Speed * dt;
        m_PlayerEntity.setPosition(position);
    }

    if (zk::Input::IsKeyDown(Key::J)) {
        zk::Vec3 position = m_PlayerEntity.getPosition();
        position -= zk::Vec3(1.0f, 0.0f, 0.0f) * m_Speed * dt;
        m_PlayerEntity.setPosition(position);
    }

    if (zk::Input::IsKeyDown(Key::I)) {
        zk::Vec3 position = m_PlayerEntity.getPosition();
        position -= zk::Vec3(0.0f, 0.0f, 1.0f) * m_Speed * dt;
        m_PlayerEntity.setPosition(position);
    }

    if (zk::Input::IsKeyDown(Key::K)) {
        zk::Vec3 position = m_PlayerEntity.getPosition();
        position += zk::Vec3(0.0f, 0.0f, 1.0f) * m_Speed * dt;
        m_PlayerEntity.setPosition(position);
    }
}
