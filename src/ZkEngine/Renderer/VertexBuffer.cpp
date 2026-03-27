#include "VertexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLVertexBuffer.hpp"
#include "ZkEngine/Renderer/Renderer.hpp"

namespace zk {

std::unique_ptr<VertexBuffer> VertexBuffer::create(std::span<const Vertex> data) {
    switch (Renderer::getAPI()) {
    case RenderAPI::OpenGL:
        return std::make_unique<OpenGLVertexBuffer>(data);
    case RenderAPI::Vulkan:
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace zk
