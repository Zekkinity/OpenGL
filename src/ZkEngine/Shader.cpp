#include "Shader.hpp"

#include "Renderer/RendererBackend.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace zk {

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
    if (!std::filesystem::exists(vertexShader)) {
        std::cout << "Shader not found: " << vertexShader << std::endl;
        return;
    } else {
        std::cout << "Loading shader: " << vertexShader << std::endl;
    }

    if (!std::filesystem::exists(fragmentShader)) {
        std::cout << "Shader not found: " << fragmentShader << std::endl;
        return;
    } else {
        std::cout << "Loading shader: " << fragmentShader << std::endl;
    }

    std::string vertexSource = readFile(vertexShader);
    std::string fragmentSource = readFile(fragmentShader);

    m_RendererID = createShader(vertexSource, fragmentSource);
}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

uint32_t Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    uint32_t shaderProgram = glCreateProgram();
    uint32_t vertShader = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    glValidateProgram(shaderProgram);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return shaderProgram;
}

uint32_t Shader::compileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int sucess;
    glGetShaderiv(id, GL_COMPILE_STATUS, &sucess);

    // Handle Shader Compilation Errors
    if (!sucess) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!\n";
        std::cout << message << '\n';
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void Shader::bind() const {
    glUseProgram(m_RendererID);
}
void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setUniform1i(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

// Set uniforms
void Shader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
    glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform3fv(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setUniform4fv(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

int Shader::getUniformLocation(const std::string& name) {
    // This will cache the uniform location so we dont have to look it up every time
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    int location = glGetUniformLocation(m_RendererID, name.data());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!\n";

    m_UniformLocationCache.emplace(std::string(name), location);
    return location;
}

std::string Shader::readFile(const std::string& filepath) {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in) {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    } else {
        std::cout << "Could not open file!\n";
    }
    return result;
}

} // namespace zk
