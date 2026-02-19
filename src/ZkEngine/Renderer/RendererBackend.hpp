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
    virtual void submit(const RenderObject& obj) = 0;
    virtual void submit(const RenderObject& obj, const Transform3D& transform) {}
    virtual void endFrame() = 0;

    virtual void setClearColor(const glm::vec4& color) = 0;
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    virtual void debug() = 0;

    virtual void drawLine(const std::vector<glm::vec3>& pts) {}
    virtual void updateCamera(const Camera3D& camera) {}

    static std::unique_ptr<RendererBackend> create(RendererAPI api);
};

} // namespace zk::Core
