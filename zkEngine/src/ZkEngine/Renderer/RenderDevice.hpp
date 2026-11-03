#pragma once

#include "ZkEngine/Core/Base.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLVertexBuffer.hpp"

#include "ZkEngine/Renderer/Vertex.hpp"
#include "ZkEngine/Renderer/VertexBuffer.hpp"
#include "ZkEngine/Renderer/IndexBuffer.hpp"

#include "ZkEngine/Renderer/Texture.hpp"
#include "ZkEngine/Utils/ImageLoader.hpp"

#include "ZkEngine/Renderer/Mesh.hpp"

#include <filesystem>
#include <vector>

namespace zk {

class Shader;
class Model;

class Framebuffer;
struct FramebufferSpec;

class Texture;
struct TextureSpecs;

struct Batch;

// NOTE:
// Render Device should not be used for asset loading, instead use AssetManager
// - maybe friend class AssetManager and private methods that we don't want to expose
class RenderDevice {
  public:
    virtual ~RenderDevice() = default;

    virtual void init() = 0;

    virtual Scope<VertexBuffer> createVertexBuffer(
        std::span<const std::byte> vertices,
        VertexBufferUsage usage = VertexBufferUsage::Static) = 0;
    virtual Scope<IndexBuffer> createIndexBuffer(std::span<const uint32_t> indices) = 0;

    virtual Ref<Mesh> createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) = 0;

    virtual Ref<Shader> createShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "") = 0;

    virtual Ref<Model> loadModel(const std::filesystem::path& path) = 0;

    virtual Scope<Framebuffer> createFramebuffer(const FramebufferSpec& specs) = 0;

    virtual void drawBatch(const Batch& batch) = 0;
    virtual void drawMesh(const Ref<Mesh>& mesh) = 0;

    virtual void drawScreenQuad() = 0;

    // 

    virtual Ref<Texture> loadTexture(
        const std::filesystem::path& path, const TextureSpecs& specs = {}) = 0;
    virtual Ref<Texture> createTexture(
        const TextureSpecs& specs, const ImageData& imageData = {}) = 0;

    virtual Ref<Texture> loadTextureHDR(const std::string& path) = 0;

    virtual Ref<Texture> loadTextureCube(const std::vector<std::filesystem::path>& faces) = 0;
    virtual Ref<Texture> createTextureCube(const TextureSpecs& specs) = 0;

};

} // namespace zk
