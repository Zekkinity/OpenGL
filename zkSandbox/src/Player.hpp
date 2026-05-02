#pragma once

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/World/Entity.hpp"

class Player {
  public:
    Player(zk::World& world);

    void onUpdate(float dt);

    zk::Entity& getEntity() { return m_PlayerEntity; }

  private:
    zk::Entity m_PlayerEntity;

    float m_Speed = 100.0f;

    // TODO:
    // Make a camera and attach it to the player
    zk::Camera3D* m_Camera = nullptr;
};
