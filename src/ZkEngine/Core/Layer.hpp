#pragma once

#include "ZkEngine/Renderer/Renderer.hpp"

namespace zk {

class Layer {
  public:
    virtual ~Layer() = default;

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(float dt) {}
    virtual void onRender(Renderer& renderer) {}
    virtual void onImGuiRender() {}
    // virtual void OnEvent(Event& event) {}
};

} // namespace zk
