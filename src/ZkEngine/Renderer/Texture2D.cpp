#include "ZkEngine/Renderer/Texture2D.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLTexture2D.hpp"

namespace zk {

std::shared_ptr<Texture2D> Texture2D::load(const std::filesystem::path& path, const TextureSpecs& specs) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL: {
        auto image = ImageLoader::load(asset(path));

        TextureSpecs finalSpecs = specs;
        finalSpecs.width = image.width;
        finalSpecs.height = image.height;

        auto texture = std::make_shared<OpenGLTexture2D>(finalSpecs, image);

        ImageLoader::free(image);

        return texture;
    }

    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanTexture>(path);
        return nullptr;
    }

    ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

std::shared_ptr<Texture2D> Texture2D::create(const TextureSpecs& specs) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return std::make_shared<OpenGLTexture2D>(specs);
    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanTexture>(specs);
        return nullptr;
    }

    ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace zk
