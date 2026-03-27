#pragma once

#include "ZkEngine/Renderer/RenderDevice.hpp"

namespace zk {

class OpenGLDevice : public RenderDevice {
  public:
    std::unique_ptr<VertexBuffer> createVertexBuffer(std::span<const Vertex> vertices) override;
    std::unique_ptr<IndexBuffer> createIndexBuffer(std::span<const uint32_t> indices) override;

    std::shared_ptr<Mesh> createMesh(
        std::vector<Vertex> vertices, std::vector<uint32_t> indices) override;

    std::shared_ptr<Shader> createShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "") override;

    std::shared_ptr<Texture> loadTexture(
        const std::filesystem::path& path, const TextureSpecs& specs = {}) override;
    std::shared_ptr<Texture> createTexture(const TextureSpecs& specs) override;

    std::shared_ptr<Texture> loadTextureCube(
        const std::vector<std::filesystem::path>& faces) override;
    std::shared_ptr<Texture> createTextureCube(const TextureSpecs& specs) override;

    std::unique_ptr<Framebuffer> createFramebuffer(const FramebufferSpec& specs) override;
};

} // namespace zk
