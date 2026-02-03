#pragma once

#include <vector>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/PhysicsSystem.hpp"
#include "ZkEngine/RenderObject.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

class Scene {
  public:
    Scene();
    virtual ~Scene();

    virtual void init();
    virtual void update(float dt);

    virtual void submit(Core::RendererBackend& renderer) const;

    void addRenderObject(std::unique_ptr<RenderObject> renderObject);
    void clearObjects();

    Camera3D* getCamera() { return m_Camera.get(); }

    virtual void debug();

  protected:
    std::vector<std::unique_ptr<RenderObject>> m_Objects;
    std::unique_ptr<Camera3D> m_Camera;
    PhysicsSystem m_PhysicsSystem;
};

} // namespace zk
