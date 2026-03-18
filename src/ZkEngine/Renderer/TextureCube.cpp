#include "ZkEngine/Renderer/TextureCube.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "ZkEngine/Core/Base.hpp"
#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLTextureCube.hpp"

namespace zk {

std::shared_ptr<Texture> TextureCube::load(const std::vector<std::filesystem::path>& faces) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL: {
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

    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanTexture>(path);
        return nullptr;
    }

    ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

std::shared_ptr<Texture> TextureCube::create(const TextureSpecs& specs) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return std::make_shared<OpenGLTextureCube>(specs);

    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanTexture>(specs);
        return nullptr;
    }

    ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace zk
