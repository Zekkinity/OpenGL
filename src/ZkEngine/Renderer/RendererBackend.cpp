#include "RendererBackend.hpp"

#include "OpenGLRenderer.hpp"
#include "ZkEngine/Renderer/VulkanRenderer.hpp"

namespace zk {

std::unique_ptr<RendererBackend> RendererBackend::create(RendererAPI api) {
    s_API = api;

    switch (api) {
    case RendererAPI::OpenGL:
        return std::make_unique<OpenGLRenderer>();
    case RendererAPI::Vulkan:
        return std::make_unique<VulkanRenderer>();
    default:
        return nullptr;
    }
}

} // namespace zk
