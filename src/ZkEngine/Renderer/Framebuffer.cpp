#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLFramebuffer.hpp"

namespace zk {
std::unique_ptr<Framebuffer> Framebuffer::create(const FramebufferSpec& specs) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return std::make_unique<OpenGLFramebuffer>(specs);
    case RendererAPI::Vulkan:
        // return std::make_unique<VulkanFramebuffer>();
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace zk
