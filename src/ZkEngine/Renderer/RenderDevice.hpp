#pragma once

#include "ZkEngine/Renderer/VertexBuffer.hpp"
#include "ZkEngine/Renderer/IndexBuffer.hpp"

#include "ZkEngine/Renderer/Mesh.hpp"

#include "ZkEngine/Renderer/Framebuffer.hpp"
#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Renderer/Texture.hpp"

#include <filesystem>

namespace zk {

class RenderDevice {
public:
    virtual std::unique_ptr<VertexBuffer> createVertexBuffer(std::span<const Vertex> vertices) = 0;
    virtual std::unique_ptr<IndexBuffer> createIndexBuffer(std::span<const uint32_t> indices) = 0;

    virtual std::shared_ptr<Mesh> createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) = 0;

    virtual std::shared_ptr<Shader> createShader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath = "") = 0;

    virtual std::shared_ptr<Texture> loadTexture(const std::filesystem::path& path, const TextureSpecs& specs = {}) = 0;
    virtual std::shared_ptr<Texture> createTexture(const TextureSpecs& specs) = 0;

    virtual std::shared_ptr<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces) = 0;
    virtual std::shared_ptr<Texture> createTextureCube(const TextureSpecs& specs) = 0;

    virtual std::unique_ptr<Framebuffer> createFramebuffer(const FramebufferSpec& specs) = 0;
};

}
