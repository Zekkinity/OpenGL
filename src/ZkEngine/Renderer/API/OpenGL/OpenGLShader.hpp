#pragma once

#include "ZkEngine/Core/Base.hpp"

#include <string>
#include <unordered_map>

#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Scene/Lights.hpp"
#include "glm/ext/matrix_float4x4.hpp"

namespace zk {

class OpenGLShader : public Shader {
  public:
    static Result<std::shared_ptr<OpenGLShader>> create(
        const std::string& vertexShader, const std::string& fragmentShader);

    ~OpenGLShader();

    void bind() const override;
    void unbind() const override;

    // Set uniforms
    void setUniform1i(const std::string& name, int value) override;
    void setUniform1f(const std::string& name, float value) override;
    void setUniform3f(const std::string& name, float v0, float v1, float v2) override;
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) override;

    void setUniform3fv(const std::string& name, const glm::vec3& value) override;

    void setUniform4fv(const std::string& name, const glm::vec4& value) override;

    void setUniformMat4f(const std::string& name, const glm::mat4& matrix) override;

    void setLightUniform(
        const std::string& name, const PointLight& light, const glm::vec3& position) override;
    void setLightUniform(const std::string& name, const DirectionalLight& light) override;
    void setLightUniform(
        const std::string& name, const SpotLight& light, const glm::vec3& position) override;

  private:
    OpenGLShader() = delete;
    OpenGLShader(uint32_t program);

    static std::string readFile(const std::string& filepath);

    static uint32_t createShader(
        const std::string& vertexShader, const std::string& fragmentShader);
    static uint32_t compileShader(uint32_t type, const std::string& source);

    int getUniformLocation(const std::string& name);

  private:
    uint32_t m_RendererID = 0;
    std::string m_FilePath;
    // Caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

} // namespace zk
