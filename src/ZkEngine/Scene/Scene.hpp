#pragma once

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/CameraController.hpp"
#include "ZkEngine/ECS/Registry.hpp"
#include "ZkEngine/ECS/Entity.hpp"

#include "ZkEngine/Scene/LightSystem.hpp"
#include "ZkEngine/Physics/PhysicsSystem.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class Scene {
  public:
    Scene();
    virtual ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    virtual void init();
    virtual void update(float dt);

    // NOTE:
    // Should be default implemented not virtual
    virtual void submit(RendererBackend& renderer) const;

    virtual void debug();

    CameraController& getCameraController() { return m_CameraController; }

    Camera3D* getCamera() { return &m_Camera; }
    const Camera3D* getCamera() const { return &m_Camera; }
    LightSystem& getLightSystem() { return m_LightSystem; }

    Registry& getRegistry() { return m_Registry; }
    const Registry& getRegistry() const { return m_Registry; }

  private:
    Registry m_Registry;
    PhysicsSystem m_PhysicsSystem;
    LightSystem m_LightSystem;

    Camera3D m_Camera;
    CameraController m_CameraController = CameraController(m_Camera);
};

} // namespace zk
