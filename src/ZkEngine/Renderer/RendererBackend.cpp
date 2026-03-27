#include "RendererBackend.hpp"

#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLRenderer.hpp"
#include "ZkEngine/Renderer/API/Vulkan/VulkanRenderer.hpp"

namespace zk {

RendererBackend::RendererBackend(RenderAPI api) : m_API(api) {}

std::unique_ptr<RendererBackend> RendererBackend::create(RenderAPI api) {
    switch (api) {
    case RenderAPI::OpenGL:
        return std::make_unique<OpenGLRenderer>();
    case RenderAPI::Vulkan:
        return std::make_unique<VulkanRenderer>();
    default:
        ZK_CORE_ASSERT(false, "Unknown RenderAPI");
        return nullptr;
    }
}

} // namespace zk
