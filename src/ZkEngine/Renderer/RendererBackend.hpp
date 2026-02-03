#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "ZkEngine/Camera3D.hpp"
#include "ZkEngine/RenderObject.hpp"

namespace zk::Core {

enum class RendererAPI { OpenGL, Vulkan };

class RendererBackend {
  public:
    virtual ~RendererBackend() = default;

    virtual void init() = 0;

    virtual void update() = 0;

    virtual void beginFrame() const = 0;
    virtual void submit(const RenderObject& obj, const Camera3D& camera) = 0;
    virtual void endFrame() = 0;

    virtual void setClearColor(const glm::vec4& color) = 0;

    virtual void debug() = 0;

    static std::unique_ptr<RendererBackend> create(RendererAPI api);
};

} // namespace zk::Core
