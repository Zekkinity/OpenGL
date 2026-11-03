#pragma once

#include "ZkEngine/Renderer/RenderDevice.hpp"

namespace zk {

class VulkanDevice : public RenderDevice {
  public:
    void init() override {}

    Ref<Shader> createShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "") override {}

    Ref<Texture> loadTexture(
        const std::filesystem::path& path, const TextureSpecs& specs = {}) override {}
    Ref<Texture> createTexture(
        const TextureSpecs& specs, const ImageData& imageData = {}) override {}

    Ref<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces) override {}
    Ref<Texture> createTextureCube(const TextureSpecs& specs) override {}

    Ref<Texture> loadTextureHDR(const std::string& path) override {}

    Scope<Framebuffer> createFramebuffer(const FramebufferSpec& specs) override {}

    Scope<VertexBuffer> createVertexBuffer(
        std::span<const std::byte> vertices,
        VertexBufferUsage usage = VertexBufferUsage::Static) override {}
    Scope<IndexBuffer> createIndexBuffer(std::span<const uint32_t> indices) override {}
    Ref<Mesh> createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) override {}
    Ref<Model> loadModel(const std::filesystem::path& path) override {}

    // Draw calls
    void drawBatch(const Batch& batch) override {}
    void drawMesh(const Ref<Mesh>& mesh) override {}
    void drawScreenQuad() override {}
};

} // namespace zk
