#pragma once

// TODO:
// Maybe this should include OpenGL specifics like glad and instead they shuld import the
// OpenGLRenderer
#include <filesystem>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "ZkEngine/Renderer/Viewport.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"
#include "ZkEngine/Camera3D.hpp"

enum class RendererAPI { OpenGL, Vulkan };

namespace zk {

class RenderPass {
  public:
    virtual ~RenderPass() = default;
    virtual void execute() = 0;
};

class Scene;

class RendererBackend {
  public:
    virtual ~RendererBackend() = default;

    virtual void init() = 0;

    virtual void update() = 0;
    virtual void submit(const RenderCommand& cmd) = 0;

    virtual void renderScene(Scene& scene) = 0;

    virtual void setClearColor(const glm::vec4& color) = 0;
    virtual void setViewport(const Viewport& viewport) = 0;

    virtual void drawLine(const std::vector<glm::vec3>& pts) {}

    virtual void debug() = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual void setSkybox(const std::vector<std::filesystem::path>& tex) {}

    static std::unique_ptr<RendererBackend> create(RendererAPI api);

    static RendererAPI getAPI() { return s_API; }

  private:
    inline static RendererAPI s_API = RendererAPI::OpenGL;
};

} // namespace zk
