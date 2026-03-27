#include "ZkEngine/Renderer/Renderer.hpp"

#include "ZkEngine/Core/Engine.hpp"
#include "ZkEngine/MeshFactory.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

static RendererBackend& Backend() {
    return Engine::Get()->getRenderer();
}

RenderDevice& Renderer::getDevice() {
    return Backend().getDevice();
}

void Renderer::init() {
    Backend().init();
}

RenderAPI Renderer::getAPI() {
    return Backend().getAPI();
}

void Renderer::update() {
    Backend().update();
}

// void Renderer::submit(const RenderObject& obj, const Camera3D& camera) {
//     Backend().submit(obj, camera);
// }

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
    Backend().setViewport(Viewport(x, y, width, height));
}

void Renderer::setSkybox(const std::vector<std::filesystem::path>& tex) {
    Backend().setSkybox(tex);
}

std::shared_ptr<Mesh> Renderer::createCube() {
    auto cubeData = MeshFactory::createCube();
    return Backend().getDevice().createMesh(cubeData.vertices, cubeData.indices);
}

std::shared_ptr<Mesh> Renderer::createSphere(float radius, int sectorCount, int stackCount) {
    auto sphereData = MeshFactory::createSphere(radius, sectorCount, stackCount);
    return Backend().getDevice().createMesh(sphereData.vertices, sphereData.indices);
}

std::shared_ptr<Mesh> Renderer::createLine() {
    auto lineData = MeshFactory::createLine();
    return Backend().getDevice().createMesh(lineData.vertices, lineData.indices);
}

std::shared_ptr<Mesh> Renderer::createTriangle() {
    auto triangleData = MeshFactory::createTriangle();
    return Backend().getDevice().createMesh(triangleData.vertices, triangleData.indices);
}

std::shared_ptr<Mesh> Renderer::createQuad() {
    auto quadData = MeshFactory::createQuad();
    return Backend().getDevice().createMesh(quadData.vertices, quadData.indices);
}

std::shared_ptr<Mesh> Renderer::createGrid(int halfSize, float spacing) {
    auto gridData = MeshFactory::createGrid(halfSize, spacing);
    return Backend().getDevice().createMesh(gridData.vertices, gridData.indices);
}


} // namespace zk
