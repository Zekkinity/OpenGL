#pragma once

#include "ZkEngine/Scene/Scene.hpp"
#include "ZkEngine/Transform3D.hpp"

#include "ZkEngine/Model.hpp"

namespace zk {

class ModelSystem {
  public:
    static void update(Scene& scene, RendererBackend& renderer) {

        for (auto& [entity, model] : scene.getRegistry().getStorage<Model>()) {

            if (model.uploaded)
                continue;

            if (!scene.getRegistry().hasComponent<Transform3D>(entity)) {
                scene.getRegistry().addComponent<Transform3D>(entity);
            }

            model.spawn(Entity(entity, scene.getRegistry()), renderer.getDevice());

            model.uploaded = true;
        }
    }
};

} // namespace zk
