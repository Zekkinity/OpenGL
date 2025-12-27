#pragma once

#include <string>
#include <unordered_map>

#include "glm/ext/matrix_float4x4.hpp"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
   public:
    Shader(const std::string& filepath);
    ~Shader();

    void bind() const;
    void unbind() const;

    // Set uniforms
    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

   private:
    ShaderProgramSource parseShader(const std::string& filepath);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    unsigned int compileShader(unsigned int type, const std::string& source);

    int getUniformLocation(const std::string& name);

   private:
    unsigned int m_RendererID;
    std::string m_FilePath;
    // Caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
};
