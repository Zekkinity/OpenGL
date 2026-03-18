#pragma once

#include "ZkEngine/Core/Base.hpp"

#include <string>
#include <unordered_map>

#include "ZkEngine/Renderer/Shader.hpp"
#include "ZkEngine/Scene/Lights.hpp"

#include <glm/glm.hpp>

namespace zk {

class OpenGLShader : public Shader {
  public:
    OpenGLShader() = delete;
    OpenGLShader(
        std::string_view vertexShader,
        std::string_view fragmentShader,
        std::string_view geometryShader = "");

    ~OpenGLShader();

    void bind() const override;
    void unbind() const override;
    void reload() override;

    // Set uniforms
    void setInt(const std::string& name, int value) override;
    void setFloat(const std::string& name, float value) override;
    void setFloat3(const std::string& name, float v0, float v1, float v2) override;
    void setFloat4(const std::string& name, float v0, float v1, float v2, float v3) override;

    void setVec3(const std::string& name, const glm::vec3& value) override;

    void setVec4(const std::string& name, const glm::vec4& value) override;

    void setMat4(const std::string& name, const glm::mat4& matrix) override;

    void setLight(
        const std::string& name, const PointLight& light, const glm::vec3& position) override;
    void setLight(const std::string& name, const DirectionalLight& light) override;
    void setLight(
        const std::string& name, const SpotLight& light, const glm::vec3& position) override;

  private:
    std::string readFile(std::string_view filepath);

    uint32_t createShader(
        std::string_view vertexShader,
        std::string_view fragmentShader,
        std::string_view geometryShader = "");
    uint32_t compileShader(uint32_t type, std::string_view source);

    int getUniformLocation(const std::string& name);

  private:
    uint32_t m_RendererID = 0;
    std::string m_VertexPath, m_FragmentPath, m_GeometryPath;

    // Caching for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

} // namespace zk
