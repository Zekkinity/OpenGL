#include "VulkanRenderer.hpp"

namespace zk {

void VulkanRenderer::init() {}
void VulkanRenderer::update() {}
void VulkanRenderer::beginFrame() const {}
void VulkanRenderer::submit(const RenderObject& obj, const Camera3D& camera) {}
void VulkanRenderer::endFrame() {}
void VulkanRenderer::setClearColor(const glm::vec4& color) {}
void VulkanRenderer::debug() {}

} // namespace zk
