#include "Shader.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"

namespace zk {

std::shared_ptr<Shader> Shader::create(
    std::string_view vertexShader,
    std::string_view fragmentShader,
    std::string_view geometryShader) {

    switch (RendererBackend::getAPI()) {
    case RendererAPI::OpenGL: {
        return std::make_shared<OpenGLShader>(vertexShader, fragmentShader, geometryShader);

        // auto shaderResult = OpenGLShader::create(vertexShader, fragmentShader);
        //
        // if (!shaderResult) {
        //     std::cout << shaderResult.error() << '\n';
        //     std::abort();
        // }
        //
        // return shaderResult.value();
    }
    case RendererAPI::Vulkan:
        // return std::make_shared<VulkanShader>(vertexShader, fragmentShader);
        return nullptr;
    }
    // ZK_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace zk
