#include "VertexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLVertexBuffer.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

namespace zk {

std::unique_ptr<VertexBuffer> VertexBuffer::create(std::span<const Vertex> data) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return std::make_unique<OpenGLVertexBuffer>(data);
    case RendererAPI::Vulkan:
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace zk
