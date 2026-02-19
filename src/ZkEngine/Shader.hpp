#pragma once

#include <string>
#include <unordered_map>

#include "ZkEngine/Scene/Lights.hpp"
#include "glm/ext/matrix_float4x4.hpp"

namespace zk {

class Shader {
   public:
    Shader(const std::string& vertexShader, const std::string& fragmentShader);
    ~Shader();

    void bind() const;
    void unbind() const;

    // Set uniforms
    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform3f(const std::string& name, float v0, float v1, float v2);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

    void setUniform3fv(const std::string& name, const glm::vec3& value);

    void setUniform4fv(const std::string& name, const glm::vec4& value);

    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

    void setLightUniform(const std::string& name, const PointLight& light, const glm::vec3& position);
    void setLightUniform(const std::string& name, const DirectionalLight& light);
    void setLightUniform(const std::string& name, const SpotLight& light, const glm::vec3& position);

   private:
    std::string readFile(const std::string& filepath);

    uint32_t createShader(const std::string& vertexShader, const std::string& fragmentShader);
    uint32_t compileShader(uint32_t type, const std::string& source);

    int getUniformLocation(const std::string& name);

   private:
    uint32_t m_RendererID = 0;
    std::string m_FilePath;
    // Caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

}
