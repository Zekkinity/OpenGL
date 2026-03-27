#include "ZkEngine/Renderer/API/OpenGL/OpenGLDevice.hpp"

#include "ZkEngine/Core/AppAcess.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLVertexBuffer.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLIndexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLMesh.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLTexture2D.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLTextureCube.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLFramebuffer.hpp"

namespace zk {

std::unique_ptr<VertexBuffer> OpenGLDevice::createVertexBuffer(std::span<const Vertex> vertices) {
    return std::make_unique<OpenGLVertexBuffer>(vertices);
}

std::unique_ptr<IndexBuffer> OpenGLDevice::createIndexBuffer(std::span<const uint32_t> indices) {
    return std::make_unique<OpenGLIndexBuffer>(indices);
}

std::shared_ptr<Mesh> OpenGLDevice::createMesh(
    std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
    return std::make_shared<OpenGLMesh>(vertices, indices);
}

std::shared_ptr<Shader> OpenGLDevice::createShader(
    std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath) {
    return std::make_shared<OpenGLShader>(vertexPath, fragmentPath, geometryPath);
}

std::shared_ptr<Texture> OpenGLDevice::loadTexture(
    const std::filesystem::path& path, const TextureSpecs& specs) {
    auto image = ImageLoader::load(asset(path));

    TextureSpecs finalSpecs = specs;
    finalSpecs.width = image.width;
    finalSpecs.height = image.height;

    auto texture = std::make_shared<OpenGLTexture2D>(finalSpecs, image);

    ImageLoader::free(image);

    return texture;
}

std::shared_ptr<Texture> OpenGLDevice::createTexture(const TextureSpecs& specs) {
    return std::make_shared<OpenGLTexture2D>(specs);
}

std::shared_ptr<Texture> OpenGLDevice::loadTextureCube(
    const std::vector<std::filesystem::path>& faces) {
    ZK_CORE_ASSERT(faces.size() == 6, "TextureCube must have 6 faces!");

    std::array<ImageData, 6> images;

    for (uint32_t i = 0; i < 6; i++) {
        images[i] = ImageLoader::load(asset(faces[i]), false);
        if (!images[i].data) {
            return nullptr;
        }
    }

    TextureSpecs specs;
    specs.width = images[0].width;
    specs.height = images[0].height;

    auto texture = std::make_shared<OpenGLTextureCube>(specs, images);
    for (auto& img : images) {
        ImageLoader::free(img);
    }

    return texture;
}

std::shared_ptr<Texture> OpenGLDevice::createTextureCube(const TextureSpecs& specs) {
    return std::make_shared<OpenGLTextureCube>(specs);
}

std::unique_ptr<Framebuffer> OpenGLDevice::createFramebuffer(const FramebufferSpec& specs) {
    return std::make_unique<OpenGLFramebuffer>(specs);
}

} // namespace zk
