#include "ZkEngine/Renderer/Texture.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLTexture.hpp"

namespace zk {

std::shared_ptr<Texture> Texture::create(const std::string& path, TextureFiltering filtering) {
    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL:
        return OpenGLTexture::create(path, filtering);
    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanTexture>(path, filtering);
        return nullptr;
    }
    // ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace zk
