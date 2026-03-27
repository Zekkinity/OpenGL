#pragma once

#include "ZkEngine/Renderer/RenderDevice.hpp"

namespace zk {

class VulkanDevice : public RenderDevice {
  public:
    std::shared_ptr<Shader> createShader(
        std::string_view vertexPath,
        std::string_view fragmentPath,
        std::string_view geometryPath = "") override {}

    std::shared_ptr<Texture> loadTexture(
        const std::filesystem::path& path, const TextureSpecs& specs = {}) override {}
    std::shared_ptr<Texture> createTexture(const TextureSpecs& specs) override {}

    std::shared_ptr<Texture> loadTextureCube(
        const std::vector<std::filesystem::path>& faces) override {}
    std::shared_ptr<Texture> createTextureCube(const TextureSpecs& specs) override {}

    std::unique_ptr<Framebuffer> createFramebuffer(const FramebufferSpec& specs) override {}
};

} // namespace zk
