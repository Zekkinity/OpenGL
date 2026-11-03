#include "zkpch.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

namespace zk {

OpenGLShader::OpenGLShader(
    std::string_view vertexShader, std::string_view fragmentShader, std::string_view geometryShader)
    : m_VertexPath(vertexShader), m_FragmentPath(fragmentShader), m_GeometryPath(geometryShader) {
    if (!std::filesystem::exists(vertexShader)) {
        ZK_CORE_ERROR("Vertex shader not found: {}", vertexShader);
    }

    if (!std::filesystem::exists(fragmentShader)) {
        ZK_CORE_ERROR("Fragment shader not found: {}", fragmentShader);
    }

    if (!std::filesystem::exists(geometryShader) && !geometryShader.empty()) {
        ZK_CORE_ERROR("Geometry shader not found: {}", geometryShader);
    }

    std::string vertexSource = readFile(vertexShader);
    std::string fragmentSource = readFile(fragmentShader);
    std::string geometrySource;
    if (std::filesystem::exists(geometryShader) && !geometryShader.empty()) {
        geometrySource = readFile(geometryShader);
    }

    m_RendererID = createShader(vertexSource, fragmentSource, geometrySource);

    if (m_RendererID == 0) {
        ZK_CORE_ERROR("Failed to create shader: {}", vertexShader);
    }
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_RendererID);
}

uint32_t OpenGLShader::createShader(
    std::string_view vertexShader,
    std::string_view fragmentShader,
    std::string_view geometryShader) {
    uint32_t shaderProgram = glCreateProgram();
    uint32_t vertShader = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    uint32_t geomShader = 0;
    if (!geometryShader.empty()) {
        geomShader = compileShader(GL_GEOMETRY_SHADER, geometryShader);
        glAttachShader(shaderProgram, geomShader);
    }

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
    if (geomShader) {
        glDeleteShader(geomShader);
    }

    return shaderProgram;
}

uint32_t OpenGLShader::compileShader(uint32_t type, std::string_view source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.data();
    const int len = source.size();
    glShaderSource(id, 1, &src, &len);
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
            type == GL_VERTEX_SHADER     ? "vertex"
            : type == GL_FRAGMENT_SHADER ? "fragment"
            : type == GL_GEOMETRY_SHADER ? "geometry"
                                         : "unknown",
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

void OpenGLShader::setInt(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void OpenGLShader::setFloat(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

// Set uniforms
void OpenGLShader::setFloat3(const std::string& name, float v0, float v1, float v2) {
    glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void OpenGLShader::setFloat4(const std::string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void OpenGLShader::setVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void OpenGLShader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void OpenGLShader::setMat4(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void OpenGLShader::setLight(
    const std::string& name, const PointLight& light, const glm::vec3& position) {
    setFloat3(name + ".position", position.x, position.y, position.z);

    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 ambient = diffuse * light.ambient;
    glm::vec3 specular = light.color * light.specular;

    setFloat3(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setFloat3(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setFloat3(name + ".specular", specular.x, specular.y, specular.z);
    setFloat(name + ".constant", light.constant);
    setFloat(name + ".linear", light.linear);
    setFloat(name + ".quadratic", light.quadratic);
    setFloat(name + ".intensity", light.intensity);
}

void OpenGLShader::setLight(const std::string& name, const DirectionalLight& light) {
    glm::vec3 ambient = light.color * light.ambient;
    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 specular = light.color * light.specular;

    setFloat3(name + ".direction", light.direction.x, light.direction.y, light.direction.z);
    setFloat3(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setFloat3(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setFloat3(name + ".specular", specular.x, specular.y, specular.z);
    setFloat(name + ".intensity", light.intensity);
}

void OpenGLShader::setLight(
    const std::string& name, const SpotLight& light, const glm::vec3& position) {
    glm::vec3 ambient = light.color * light.ambient;
    glm::vec3 diffuse = light.color * light.diffuse;
    glm::vec3 specular = light.color * light.specular;

    setFloat3(name + ".position", position.x, position.y, position.z);
    setFloat3(name + ".direction", light.direction.x, light.direction.y, light.direction.z);
    setFloat(name + ".cutOff", light.cutOff);
    setFloat(name + ".outerCutOff", light.outerCutOff);
    setFloat3(name + ".ambient", ambient.x, ambient.y, ambient.z);
    setFloat3(name + ".diffuse", diffuse.x, diffuse.y, diffuse.z);
    setFloat3(name + ".specular", specular.x, specular.y, specular.z);
    setFloat(name + ".constant", light.constant);
    setFloat(name + ".linear", light.linear);
    setFloat(name + ".quadratic", light.quadratic);
    setFloat(name + ".intensity", light.intensity);
}

void OpenGLShader::reload() {
    std::string vertexSource = readFile(m_VertexPath);
    std::string fragmentSource = readFile(m_FragmentPath);
    std::string geometrySource;
    if (!m_GeometryPath.empty()) {
        geometrySource = readFile(m_GeometryPath);
    }

    uint32_t program = createShader(vertexSource, fragmentSource, geometrySource);

    if (program == 0) {
        ZK_CORE_ERROR("Failed to reload shader");
        return;
    }

    glDeleteProgram(m_RendererID);
    m_RendererID = program;

    m_UniformLocationCache.clear();

    ZK_CORE_DEBUG("Shader reloaded");
}

int OpenGLShader::getUniformLocation(const std::string& name) {
    // This will cache the uniform location so we dont have to look it up every time
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        ZK_CORE_DEBUG("Uniform '{}' not found in shader {}", name, m_RendererID);
        // return -1;
    }

    m_UniformLocationCache.emplace(std::string(name), location);
    return location;
}

std::string OpenGLShader::readFile(std::string_view filepath) {
    std::ifstream in(filepath.data(), std::ios::binary);

    if (!in) {
        ZK_CORE_ERROR("Could not open file: {}", filepath);
        return {};
    }

    return std::string((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
}

} // namespace zk
