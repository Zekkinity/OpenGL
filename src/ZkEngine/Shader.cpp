#include "Shader.hpp"

#include "Renderer/Renderer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0) {
    ShaderProgramSource source = parseShader(filepath);
    m_RendererID = createShader(source.VertexSource, source.FragmentSource);
}
Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

unsigned int Shader::createShader(const std::string& vertexShader,
                                  const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// uint32_t Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
//     uint32_t shaderProgram = glCreateProgram();
//     uint32_t vertShader = compileVertexShader(vertexShader);
//     uint32_t fragShader = compileFragmentShader(fragmentShader);
//
//     glAttachShader(shaderProgram, vertShader);
//     glAttachShader(shaderProgram, fragShader);
//     glLinkProgram(shaderProgram);
//
//     glValidateProgram(shaderProgram);
//
//     glDeleteShader(vertShader);
//     glDeleteShader(fragShader);
//
//     return shaderProgram;
// }
//
// uint32_t Shader::compileFragmentShader(const std::string& source) {
//     uint32_t fragmentShader;
//     fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     const char* src = source.c_str();
//     glShaderSource(fragmentShader, 1, &src, nullptr);
//     glCompileShader(fragmentShader);
// }

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
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
    // This will cache the uniform location so we don't have to look it up every time
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    int location = glGetUniformLocation(m_RendererID, name.data());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!\n";

    m_UniformLocationCache.emplace(std::string(name), location);
    return location;
}

// Get shaders from filepath
ShaderProgramSource Shader::parseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1,
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }
    return {ss[0].str(), ss[1].str()};
}
