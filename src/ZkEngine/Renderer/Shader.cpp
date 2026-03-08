#include "Shader.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"
#include <iostream>

namespace zk {

std::shared_ptr<Shader> Shader::create(
    const std::string& vertexShader, const std::string& fragmentShader) {

    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL: {
        auto shaderResult = OpenGLShader::create(vertexShader, fragmentShader);

        if (!shaderResult) {
            std::cout << shaderResult.error() << '\n';
            std::abort();
        }

        return shaderResult.value();
    }
    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanShader>(vertexShader, fragmentShader);
        return nullptr;
    }
    // ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace zk
