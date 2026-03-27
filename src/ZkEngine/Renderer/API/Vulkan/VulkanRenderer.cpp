#include "VulkanRenderer.hpp"

namespace zk {

VulkanRenderer::VulkanRenderer() : RendererBackend(RenderAPI::Vulkan) {}

void VulkanRenderer::init() {}
void VulkanRenderer::update() {}
void VulkanRenderer::renderScene(Scene& scene) {}

void VulkanRenderer::submit(const RenderCommand& cmd) {}
void VulkanRenderer::setClearColor(const glm::vec4& color) {}
void VulkanRenderer::setViewport(const Viewport& viewport) {}
void VulkanRenderer::debug() {}
void VulkanRenderer::resize(uint32_t width, uint32_t height) {}

} // namespace zk
