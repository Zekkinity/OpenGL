#include "ZkEngine/Renderer/Framebuffer.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLFramebuffer.hpp"

namespace zk {
std::unique_ptr<Framebuffer> Framebuffer::create() {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return OpenGLFramebuffer::create();
    case RendererAPI::Vulkan:
        // return std::make_unique<VulkanFramebuffer>();
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace zk
