#include "VulkanRenderer.hpp"

namespace zk {

void VulkanRenderer::init() {}
void VulkanRenderer::update() {}
void VulkanRenderer::beginFrame() const {}
void VulkanRenderer::submit(const RenderObject& obj) {}
void VulkanRenderer::endFrame() {}
void VulkanRenderer::setClearColor(const glm::vec4& color) {}
void VulkanRenderer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
void VulkanRenderer::debug() {}

} // namespace zk
