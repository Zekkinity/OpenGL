#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"

#include "ZkEngine/Renderer/RendererBackend.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace zk {

Result<std::shared_ptr<OpenGLShader>> OpenGLShader::create(
    const std::string& vertexShader, const std::string& fragmentShader) {
    if (!std::filesystem::exists(vertexShader)) {
        return Err("Vertex shader not found: " + vertexShader);
    }

    if (!std::filesystem::exists(fragmentShader)) {
        return Err("Fragment shader not found: " + fragmentShader);
    }

    std::string vertexSource = readFile(vertexShader);
    std::string fragmentSource = readFile(fragmentShader);

    uint32_t program = createShader(vertexSource, fragmentSource);

    if (program == 0) {
        return Err("Failed to create shader");
    }

    // Call new so we can keep constructor private
    return std::shared_ptr<OpenGLShader>(new OpenGLShader(program));
}

OpenGLShader::OpenGLShader(uint32_t program) : m_RendererID(program) {}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_RendererID);
}

uint32_t OpenGLShader::createShader(
    const std::string& vertexShader, const std::string& fragmentShader) {
    uint32_t shaderProgram = glCreateProgram();
    uint32_t vertShader = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    int sucess;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &sucess);

    if (!sucess) {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message(length);
        glGetProgramInfoLog(shaderProgram, length, &length, message.data());

        // Remove annoying trailing newline
        std::string msg(message.data());
        if (!msg.empty() && msg.back() == '\n')
            msg.pop_back();

        ZK_CORE_ERROR("Shader link failed: {}", msg);

        glDeleteProgram(shaderProgram);
        return 0;
    }

    glValidateProgram(shaderProgram);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return shaderProgram;
}

uint32_t OpenGLShader::compileShader(uint32_t type, const std::string& source) {
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
        std::vector<char> message(length);
        glGetShaderInfoLog(id, length, &length, message.data());

        // Remove annoying trailing newline
        std::string msg(message.data());
        if (!msg.empty() && msg.back() == '\n')
            msg.pop_back();

        ZK_CORE_ERROR(
            "Failed to compile {} shader!: {}",
            type == GL_VERTEX_SHADER ? "vertex" : "fragment",
            msg);

        glDeleteShader(id);
        return 0;
    }

    return id;
}

void OpenGLShader::bind() const {
    glUseProgram(m_RendererID);
}
void OpenGLShader::unbind() const {
    glUseProgram(0);
}

void OpenGLShader::setUniform1i(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void OpenGLShader::setUniform1f(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

// Set uniforms
void OpenGLShader::setUniform3f(const std::string& name, float v0, float v1, float v2) {
    glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void OpenGLShader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void OpenGLShader::setUniform3fv(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void OpenGLShader::setUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void OpenGLShader::setUniform4fv(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void OpenGLShader::setLightUniform(
    const std::string& name, const PointLight& light, const glm::vec3& position) {
    setUniform3f(name + ".position", position.x, position.y, position.z);

    //     NOTE:
    //     glm::vec3 diffuseColor = m_LightColor * glm::vec3(0.8f);
    // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 ambient = diffuse * light.ambient;
    glm::vec3 specular = light.color * light.specular;

    setUniform3f(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setUniform3f(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setUniform3f(name + ".specular", specular.x, specular.y, specular.z);
    setUniform1f(name + ".constant", light.constant);
    setUniform1f(name + ".linear", light.linear);
    setUniform1f(name + ".quadratic", light.quadratic);
}

void OpenGLShader::setLightUniform(const std::string& name, const DirectionalLight& light) {
    glm::vec3 ambient = light.color * light.ambient;
    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 specular = light.color * light.specular;

    setUniform3f(name + ".direction", light.direction.x, light.direction.y, light.direction.z);
    setUniform3f(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setUniform3f(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setUniform3f(name + ".specular", specular.x, specular.y, specular.z);
}

void OpenGLShader::setLightUniform(
    const std::string& name, const SpotLight& light, const glm::vec3& position) {
    glm::vec3 ambient = light.color * light.ambient;
    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 specular = light.color * light.specular;

    setUniform3f(name + ".position", position.x, position.y, position.z);
    setUniform3f(name + ".direction", light.direction.x, light.direction.y, light.direction.z);
    setUniform1f(name + ".cutOff", light.cutOff);
    setUniform1f(name + ".outerCutOff", light.outerCutOff);
    setUniform3f(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setUniform3f(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setUniform3f(name + ".specular", specular.x, specular.y, specular.z);
    setUniform1f(name + ".constant", light.constant);
    setUniform1f(name + ".linear", light.linear);
    setUniform1f(name + ".quadratic", light.quadratic);
}

int OpenGLShader::getUniformLocation(const std::string& name) {
    // This will cache the uniform location so we dont have to look it up every time
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        ZK_CORE_WARN("Uniform '{}' not found in shader {}", name, m_RendererID);

    m_UniformLocationCache.emplace(std::string(name), location);
    return location;
}

std::string OpenGLShader::readFile(const std::string& filepath) {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in) {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else {
        ZK_CORE_ERROR("Could not open file: {}", filepath);
    }
    return result;
}

} // namespace zk
