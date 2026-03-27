#pragma once

#include <glm/glm.hpp>

#include <filesystem>

#include "ZkEngine/Renderer/RenderDevice.hpp"
#include "ZkEngine/Renderer/Viewport.hpp"
#include "ZkEngine/Renderer/Core/RenderCommand.hpp"

enum class RenderAPI { OpenGL, Vulkan };

namespace zk {

class Scene;

class RendererBackend {
  public:
    RendererBackend(RenderAPI api);
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

    static std::unique_ptr<RendererBackend> create(RenderAPI api);

    virtual RenderDevice& getDevice() = 0;

    virtual RenderAPI getAPI() const { return m_API; }

  private:
    RenderAPI m_API = RenderAPI::OpenGL;
};

} // namespace zk
