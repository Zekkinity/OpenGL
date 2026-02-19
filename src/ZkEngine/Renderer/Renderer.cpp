#include "ZkEngine/Renderer/Renderer.hpp"

#include "ZkEngine/Core/Engine.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

static Core::RendererBackend& Backend() {
    return Engine::Get()->getRenderer();
}

void Renderer::init() {
    Backend().init();
}

void Renderer::update() {
    Backend().update();
}

void Renderer::beginFrame() {
    Backend().beginFrame();
}

// void Renderer::submit(const RenderObject& obj, const Camera3D& camera) {
//     Backend().submit(obj, camera);
// }

void Renderer::endFrame() {
    Backend().endFrame();
}

void Renderer::setClearColor(const glm::vec4& color) {
    Backend().setClearColor(color);
}

void Renderer::debug() {
    Backend().debug();
}

void Renderer::drawLine(const std::vector<glm::vec3>& pts) {
    Backend().drawLine(pts);
}

void Renderer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    Backend().setViewport(x, y, width, height);
}

} // namespace zk
